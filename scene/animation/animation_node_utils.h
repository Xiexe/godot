/**************************************************************************/
/*  animation_node_utils.h                                                */
/**************************************************************************/
#pragma once

#include "scene/animation/animation_blend_tree.h"
#include "scene/animation/animation_mixer.h"
#include "scene/resources/animation.h"
#include "core/io/resource_loader.h"

class AnimationNodeUtils {
public:
	static double get_node_length(const Ref<AnimationNode> &p_node, AnimationMixer *p_mixer);

	struct NormalizedSyncState {
		bool enabled = false;      // true when SYNC_MODE_NORMALIZED is active
		double virtual_len = 1.0;  // weighted average length
		bool do_resync = false;    // one-shot resync on mode change
		double resync_phase = 0.0; // [0,1] phase
	};

	typedef double (*BlendSpaceLengthFunc)(void *p_userdata, int p_index, AnimationMixer *p_mixer);
	typedef AnimationNode::NodeTimeInfo (*BlendSpacePeekFunc)(void *p_userdata, int p_index, const AnimationMixer::PlaybackInfo &p_pi);

	// NOTE: p_force_resync is true on the frame where sync_mode just changed.
	static NormalizedSyncState prepare_normalized_sync_state(
			bool p_is_normalized,
			bool p_force_resync,
			const float *p_weights,
			int p_blend_points_used,
			int p_master_idx,
			AnimationNode::ProcessState *p_process_state,
			const AnimationMixer::PlaybackInfo &p_pi,
			void *p_userdata,
			BlendSpaceLengthFunc p_length_func,
			BlendSpacePeekFunc p_peek_func
	);

	static void apply_normalized_sync_to_child(
			const NormalizedSyncState &p_state,
			AnimationMixer::PlaybackInfo &r_pi_child,
			const AnimationMixer::PlaybackInfo &p_pi_parent,
			double p_child_len
	);
};
