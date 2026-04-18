/**************************************************************************/
/*  animation_node_utils.cpp                                              */
/**************************************************************************/

#include "animation_node_utils.h"

#include "scene/animation/animation_blend_tree.h"
#include "scene/animation/animation_mixer.h"
#include "scene/resources/animation.h"
#include "core/io/resource_loader.h"

// Recursive helper to find the actual animation node driving a branch
static Ref<AnimationNodeAnimation> _find_anim_node(const Ref<AnimationNode> &p_node, int p_depth = 0) {
	if (p_node.is_null() || p_depth > 10) {
		return Ref<AnimationNodeAnimation>();
	}

	// Direct Match
	Ref<AnimationNodeAnimation> anim_node = p_node;
	if (anim_node.is_valid()) {
		return anim_node;
	}

	// BlendTree: Follow Output
	Ref<AnimationNodeBlendTree> tree = p_node;
	if (tree.is_valid()) {
		List<AnimationNodeBlendTree::NodeConnection> connections;
		tree->get_node_connections(&connections);
		for (const AnimationNodeBlendTree::NodeConnection &E : connections) {
			if (E.input_node == "output") {
				if (tree->has_node(E.output_node)) {
					return _find_anim_node(tree->get_node(E.output_node), p_depth + 1);
				}
			}
		}
		return Ref<AnimationNodeAnimation>();
	}

	// Generic Children (TimeScale, Blend2, etc): Check first valid child
	List<AnimationNode::ChildNode> children;
	p_node->get_child_nodes(&children);
	for (const AnimationNode::ChildNode &E : children) {
		Ref<AnimationNodeAnimation> found = _find_anim_node(E.node, p_depth + 1);
		if (found.is_valid()) {
			return found;
		}
	}

	return Ref<AnimationNodeAnimation>();
}

double AnimationNodeUtils::get_node_length(const Ref<AnimationNode> &p_node, AnimationMixer *p_mixer) {
	Ref<AnimationNodeAnimation> anim_node = _find_anim_node(p_node);

	if (anim_node.is_valid()) {
		if (anim_node->is_using_custom_timeline()) {
			double t_len = anim_node->get_timeline_length();
			if (t_len > 0.0001) {
				return t_len;
			}
		}

		StringName anim_name = anim_node->get_animation();
		Ref<Animation> anim;

		if (p_mixer) {
			anim = p_mixer->get_animation(anim_name);
		}

		if (anim.is_null() && String(anim_name).is_absolute_path()) {
			anim = ResourceLoader::load(anim_name);
		}

		if (anim.is_valid()) {
			double len = anim->get_length();
			return (len > 0.0001) ? len : 1.0;
		}
	}
	return 1.0;
}

AnimationNodeUtils::NormalizedSyncState AnimationNodeUtils::prepare_normalized_sync_state(
		bool p_is_normalized,
		bool p_force_resync,
		const float *p_weights,
		int p_blend_points_used,
		int p_master_idx,
		AnimationNode::ProcessState *p_process_state,
		const AnimationMixer::PlaybackInfo &p_pi,
		void *p_userdata,
		BlendSpaceLengthFunc p_length_func,
		BlendSpacePeekFunc p_peek_func) {

	NormalizedSyncState s;

	// Only does anything when caller says we're in "normalized" mode.
	if (!p_is_normalized) {
		return s; // enabled == false
	}

	s.enabled = true;

	AnimationMixer *mixer = p_process_state ? p_process_state->tree : nullptr;

	// Compute virtual length (weighted average of active clips).
	double w_sum = 0.0;
	double len_sum = 0.0;

	for (int i = 0; i < p_blend_points_used; i++) {
		if (p_weights[i] > CMP_EPSILON) {
			double l = p_length_func(p_userdata, i, mixer);
			len_sum += l * p_weights[i];
			w_sum += p_weights[i];
		}
	}

	s.virtual_len = (w_sum > 0.0) ? (len_sum / w_sum) : 1.0;
	if (s.virtual_len < 0.001) {
		s.virtual_len = 1.0;
	}

	// One-frame resync when caller requests it (mode change into normalized).
	if (p_force_resync && p_master_idx != -1 && mixer) {
		double m_len = p_length_func(p_userdata, p_master_idx, mixer);
		if (m_len > 0.001) {
			AnimationMixer::PlaybackInfo pi_peek = p_pi;
			pi_peek.weight = 0.0;
			AnimationNode::NodeTimeInfo t_master = p_peek_func(p_userdata, p_master_idx, pi_peek);
			s.resync_phase = t_master.position / m_len;
			s.do_resync = true;
		}
	}

	return s;
}


void AnimationNodeUtils::apply_normalized_sync_to_child(
		const NormalizedSyncState &p_state,
		AnimationMixer::PlaybackInfo &r_pi_child,
		const AnimationMixer::PlaybackInfo &p_pi_parent,
		double p_child_len) {

	if (!p_state.enabled) {
		return;
	}

	if (p_state.do_resync) {
		// On mode change, force all clips to the same phase.
		r_pi_child.seeked = true;
		r_pi_child.time = p_state.resync_phase * p_child_len;
	} else if (p_pi_parent.seeked) {
		// External seek: parent time is "virtual" time.
		const double phase = (p_state.virtual_len > 0.0) ? (p_pi_parent.time / p_state.virtual_len) : 0.0;
		r_pi_child.seeked = true;
		r_pi_child.time = phase * p_child_len;
	} else {
		// Normal speed normalization.
		const double scale = p_child_len / p_state.virtual_len;
		r_pi_child.delta *= scale;
		r_pi_child.time *= scale;
	}
}
