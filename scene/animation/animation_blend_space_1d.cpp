/**************************************************************************/
/*  animation_blend_space_1d.cpp                                          */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "animation_blend_space_1d.h"

#include "animation_blend_tree.h"
#include "animation_node_utils.h"

void AnimationNodeBlendSpace1D::get_parameter_list(List<PropertyInfo> *r_list) const {
	AnimationNode::get_parameter_list(r_list);
	r_list->push_back(PropertyInfo(Variant::FLOAT, blend_position));
	r_list->push_back(PropertyInfo(Variant::INT, closest, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE));
}

Variant AnimationNodeBlendSpace1D::get_parameter_default_value(const StringName &p_parameter) const {
	Variant ret = AnimationNode::get_parameter_default_value(p_parameter);
	if (ret != Variant()) {
		return ret;
	}

	if (p_parameter == closest) {
		return (int)-1;
	} else {
		return 0.0;
	}
}

Ref<AnimationNode> AnimationNodeBlendSpace1D::get_child_by_name(const StringName &p_name) const {
	return get_blend_point_node(p_name.operator String().to_int());
}

void AnimationNodeBlendSpace1D::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name.begins_with("blend_point_")) {
		String left = p_property.name.get_slicec('/', 0);
		int idx = left.get_slicec('_', 2).to_int();
		if (idx >= blend_points_used) {
			p_property.usage = PROPERTY_USAGE_NONE;
		}
	}
}

void AnimationNodeBlendSpace1D::_tree_changed() {
	AnimationRootNode::_tree_changed();
}

void AnimationNodeBlendSpace1D::_animation_node_renamed(const ObjectID &p_oid, const String &p_old_name, const String &p_new_name) {
	AnimationRootNode::_animation_node_renamed(p_oid, p_old_name, p_new_name);
}

void AnimationNodeBlendSpace1D::_animation_node_removed(const ObjectID &p_oid, const StringName &p_node) {
	AnimationRootNode::_animation_node_removed(p_oid, p_node);
}

void AnimationNodeBlendSpace1D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_blend_point", "node", "pos", "at_index"), &AnimationNodeBlendSpace1D::add_blend_point, DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("set_blend_point_position", "point", "pos"), &AnimationNodeBlendSpace1D::set_blend_point_position);
	ClassDB::bind_method(D_METHOD("get_blend_point_position", "point"), &AnimationNodeBlendSpace1D::get_blend_point_position);
	ClassDB::bind_method(D_METHOD("set_blend_point_node", "point", "node"), &AnimationNodeBlendSpace1D::set_blend_point_node);
	ClassDB::bind_method(D_METHOD("get_blend_point_node", "point"), &AnimationNodeBlendSpace1D::get_blend_point_node);
	ClassDB::bind_method(D_METHOD("remove_blend_point", "point"), &AnimationNodeBlendSpace1D::remove_blend_point);
	ClassDB::bind_method(D_METHOD("get_blend_point_count"), &AnimationNodeBlendSpace1D::get_blend_point_count);

	ClassDB::bind_method(D_METHOD("set_min_space", "min_space"), &AnimationNodeBlendSpace1D::set_min_space);
	ClassDB::bind_method(D_METHOD("get_min_space"), &AnimationNodeBlendSpace1D::get_min_space);

	ClassDB::bind_method(D_METHOD("set_max_space", "max_space"), &AnimationNodeBlendSpace1D::set_max_space);
	ClassDB::bind_method(D_METHOD("get_max_space"), &AnimationNodeBlendSpace1D::get_max_space);

	ClassDB::bind_method(D_METHOD("set_snap", "snap"), &AnimationNodeBlendSpace1D::set_snap);
	ClassDB::bind_method(D_METHOD("get_snap"), &AnimationNodeBlendSpace1D::get_snap);

	ClassDB::bind_method(D_METHOD("set_value_label", "text"), &AnimationNodeBlendSpace1D::set_value_label);
	ClassDB::bind_method(D_METHOD("get_value_label"), &AnimationNodeBlendSpace1D::get_value_label);

	ClassDB::bind_method(D_METHOD("set_blend_mode", "mode"), &AnimationNodeBlendSpace1D::set_blend_mode);
	ClassDB::bind_method(D_METHOD("get_blend_mode"), &AnimationNodeBlendSpace1D::get_blend_mode);

	ClassDB::bind_method(D_METHOD("set_sync_mode", "mode"), &AnimationNodeBlendSpace1D::set_sync_mode);
	ClassDB::bind_method(D_METHOD("get_sync_mode"), &AnimationNodeBlendSpace1D::get_sync_mode);

	ClassDB::bind_method(D_METHOD("_add_blend_point", "index", "node"), &AnimationNodeBlendSpace1D::_add_blend_point);

	for (int i = 0; i < MAX_BLEND_POINTS; i++) {
		ADD_PROPERTYI(PropertyInfo(Variant::OBJECT, "blend_point_" + itos(i) + "/node", PROPERTY_HINT_RESOURCE_TYPE, "AnimationRootNode", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL), "_add_blend_point", "get_blend_point_node", i);
		ADD_PROPERTYI(PropertyInfo(Variant::FLOAT, "blend_point_" + itos(i) + "/pos", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL), "set_blend_point_position", "get_blend_point_position", i);
	}

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_space", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_min_space", "get_min_space");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_space", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_max_space", "get_max_space");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "snap", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_snap", "get_snap");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "value_label", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_value_label", "get_value_label");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "blend_mode", PROPERTY_HINT_ENUM, "Interpolated,Discrete,Carry", PROPERTY_USAGE_NO_EDITOR), "set_blend_mode", "get_blend_mode");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "sync_mode", PROPERTY_HINT_ENUM, "None,Absolute,Normalized", PROPERTY_USAGE_NO_EDITOR), "set_sync_mode", "get_sync_mode");

	BIND_ENUM_CONSTANT(BLEND_MODE_INTERPOLATED);
	BIND_ENUM_CONSTANT(BLEND_MODE_DISCRETE);
	BIND_ENUM_CONSTANT(BLEND_MODE_DISCRETE_CARRY);

	BIND_ENUM_CONSTANT(SYNC_MODE_NONE);
	BIND_ENUM_CONSTANT(SYNC_MODE_ABSOLUTE);
	BIND_ENUM_CONSTANT(SYNC_MODE_NORMALIZED);
}

void AnimationNodeBlendSpace1D::get_child_nodes(List<ChildNode> *r_child_nodes) {
	for (int i = 0; i < blend_points_used; i++) {
		ChildNode cn;
		cn.name = itos(i);
		cn.node = blend_points[i].node;
		r_child_nodes->push_back(cn);
	}
}

void AnimationNodeBlendSpace1D::add_blend_point(const Ref<AnimationRootNode> &p_node, float p_position, int p_at_index) {
	ERR_FAIL_COND(blend_points_used >= MAX_BLEND_POINTS);
	ERR_FAIL_COND(p_node.is_null());

	ERR_FAIL_COND(p_at_index < -1 || p_at_index > blend_points_used);

	if (p_at_index == -1 || p_at_index == blend_points_used) {
		p_at_index = blend_points_used;
	} else {
		for (int i = blend_points_used - 1; i > p_at_index; i--) {
			blend_points[i] = blend_points[i - 1];
		}
	}

	blend_points[p_at_index].node = p_node;
	blend_points[p_at_index].position = p_position;

	blend_points[p_at_index].node->connect("tree_changed", callable_mp(this, &AnimationNodeBlendSpace1D::_tree_changed), CONNECT_REFERENCE_COUNTED);
	blend_points[p_at_index].node->connect("animation_node_renamed", callable_mp(this, &AnimationNodeBlendSpace1D::_animation_node_renamed), CONNECT_REFERENCE_COUNTED);
	blend_points[p_at_index].node->connect("animation_node_removed", callable_mp(this, &AnimationNodeBlendSpace1D::_animation_node_removed), CONNECT_REFERENCE_COUNTED);

	blend_points_used++;
	emit_signal(SNAME("tree_changed"));

	pending_resync = true;
}

void AnimationNodeBlendSpace1D::set_blend_point_position(int p_point, float p_position) {
	ERR_FAIL_INDEX(p_point, blend_points_used);

	blend_points[p_point].position = p_position;

	pending_resync = true;
}

void AnimationNodeBlendSpace1D::set_blend_point_node(int p_point, const Ref<AnimationRootNode> &p_node) {
	ERR_FAIL_INDEX(p_point, blend_points_used);
	ERR_FAIL_COND(p_node.is_null());

	if (blend_points[p_point].node.is_valid()) {
		blend_points[p_point].node->disconnect("tree_changed", callable_mp(this, &AnimationNodeBlendSpace1D::_tree_changed));
		blend_points[p_point].node->disconnect("animation_node_renamed", callable_mp(this, &AnimationNodeBlendSpace1D::_animation_node_renamed));
		blend_points[p_point].node->disconnect("animation_node_removed", callable_mp(this, &AnimationNodeBlendSpace1D::_animation_node_removed));
	}

	blend_points[p_point].node = p_node;
	blend_points[p_point].node->connect("tree_changed", callable_mp(this, &AnimationNodeBlendSpace1D::_tree_changed), CONNECT_REFERENCE_COUNTED);
	blend_points[p_point].node->connect("animation_node_renamed", callable_mp(this, &AnimationNodeBlendSpace1D::_animation_node_renamed), CONNECT_REFERENCE_COUNTED);
	blend_points[p_point].node->connect("animation_node_removed", callable_mp(this, &AnimationNodeBlendSpace1D::_animation_node_removed), CONNECT_REFERENCE_COUNTED);

	emit_signal(SNAME("tree_changed"));

	pending_resync = true;
}

float AnimationNodeBlendSpace1D::get_blend_point_position(int p_point) const {
	ERR_FAIL_INDEX_V(p_point, MAX_BLEND_POINTS, 0);
	return blend_points[p_point].position;
}

Ref<AnimationRootNode> AnimationNodeBlendSpace1D::get_blend_point_node(int p_point) const {
	ERR_FAIL_INDEX_V(p_point, MAX_BLEND_POINTS, Ref<AnimationRootNode>());
	return blend_points[p_point].node;
}

void AnimationNodeBlendSpace1D::remove_blend_point(int p_point) {
	ERR_FAIL_INDEX(p_point, blend_points_used);

	ERR_FAIL_COND(blend_points[p_point].node.is_null());
	blend_points[p_point].node->disconnect("tree_changed", callable_mp(this, &AnimationNodeBlendSpace1D::_tree_changed));
	blend_points[p_point].node->disconnect("animation_node_renamed", callable_mp(this, &AnimationNodeBlendSpace1D::_animation_node_renamed));
	blend_points[p_point].node->disconnect("animation_node_removed", callable_mp(this, &AnimationNodeBlendSpace1D::_animation_node_removed));

	for (int i = p_point; i < blend_points_used - 1; i++) {
		blend_points[i] = blend_points[i + 1];
	}

	blend_points_used--;

	emit_signal(SNAME("animation_node_removed"), get_instance_id(), itos(p_point));
	emit_signal(SNAME("tree_changed"));

	pending_resync = true;
}

int AnimationNodeBlendSpace1D::get_blend_point_count() const {
	return blend_points_used;
}

void AnimationNodeBlendSpace1D::set_min_space(float p_min) {
	min_space = p_min;

	if (min_space >= max_space) {
		min_space = max_space - 1;
	}
}

float AnimationNodeBlendSpace1D::get_min_space() const {
	return min_space;
}

void AnimationNodeBlendSpace1D::set_max_space(float p_max) {
	max_space = p_max;

	if (max_space <= min_space) {
		max_space = min_space + 1;
	}
}

float AnimationNodeBlendSpace1D::get_max_space() const {
	return max_space;
}

void AnimationNodeBlendSpace1D::set_snap(float p_snap) {
	snap = p_snap;
}

float AnimationNodeBlendSpace1D::get_snap() const {
	return snap;
}

void AnimationNodeBlendSpace1D::set_value_label(const String &p_label) {
	value_label = p_label;
}

String AnimationNodeBlendSpace1D::get_value_label() const {
	return value_label;
}

void AnimationNodeBlendSpace1D::set_blend_mode(BlendMode p_blend_mode) {
	blend_mode = p_blend_mode;
}

AnimationNodeBlendSpace1D::BlendMode AnimationNodeBlendSpace1D::get_blend_mode() const {
	return blend_mode;
}

void AnimationNodeBlendSpace1D::set_sync_mode(SyncMode p_mode) {
	sync_mode = p_mode;
	pending_resync = true;
}

AnimationNodeBlendSpace1D::SyncMode AnimationNodeBlendSpace1D::get_sync_mode() const {
	return sync_mode;
}

void AnimationNodeBlendSpace1D::_add_blend_point(int p_index, const Ref<AnimationRootNode> &p_node) {
	if (p_index == blend_points_used) {
		add_blend_point(p_node, 0);
	} else {
		set_blend_point_node(p_index, p_node);
	}
}

double _blend_space_1d_get_length(void *p_userdata, int p_index, AnimationMixer *p_mixer) {
	AnimationNodeBlendSpace1D *self = static_cast<AnimationNodeBlendSpace1D *>(p_userdata);
	return AnimationNodeUtils::get_node_length(self->blend_points[p_index].node, p_mixer);
}

AnimationNode::NodeTimeInfo _blend_space_1d_peek(
		void *p_userdata,
		int p_index,
		const AnimationMixer::PlaybackInfo &p_pi) {

	AnimationNodeBlendSpace1D *self = static_cast<AnimationNodeBlendSpace1D *>(p_userdata);
	AnimationNodeBlendSpace1D::BlendPoint &bp = self->blend_points[p_index];

	return self->blend_node(bp.node, bp.name, p_pi, AnimationNode::FILTER_IGNORE, true, true);
}

AnimationNode::NodeTimeInfo AnimationNodeBlendSpace1D::_process(const AnimationMixer::PlaybackInfo p_playback_info, bool p_test_only) {
	if (!blend_points_used) {
		return NodeTimeInfo();
	}

	AnimationMixer::PlaybackInfo pi = p_playback_info;

	if (blend_points_used == 1) {
		pi.weight = 1.0;
		return blend_node(blend_points[0].node, blend_points[0].name, pi, FILTER_IGNORE, true, p_test_only);
	}

	double blend_pos = get_parameter(blend_position);
	int cur_closest = get_parameter(closest);
	NodeTimeInfo mind;

	float weights[MAX_BLEND_POINTS] = {};
	int master_idx = -1;
	float max_weight = -1.0f;

	// --- Weights ---
	if (blend_mode == BLEND_MODE_INTERPOLATED) {
		int point_lower = -1;
		float pos_lower = 0.0f;
		int point_higher = -1;
		float pos_higher = 0.0f;

		for (int i = 0; i < blend_points_used; i++) {
			float pos = blend_points[i].position;
			if (pos <= blend_pos) {
				if (point_lower == -1 || pos > pos_lower) {
					point_lower = i;
					pos_lower = pos;
				}
			} else {
				if (point_higher == -1 || pos < pos_higher) {
					point_higher = i;
					pos_higher = pos;
				}
			}
		}

		if (point_lower == -1 && point_higher != -1) {
			weights[point_higher] = 1.0f;
		} else if (point_higher == -1) {
			weights[point_lower] = 1.0f;
		} else {
			float dist = pos_higher - pos_lower;
			float ratio = (blend_pos - pos_lower) / dist;
			weights[point_lower] = 1.0f - ratio;
			weights[point_higher] = ratio;
		}
	} else {
		// Discrete / carry logic.
		double new_closest_dist = 1e20;
		int new_closest = -1;

		for (int i = 0; i < blend_points_used; i++) {
			double d = Math::abs(blend_points[i].position - blend_pos);
			if (d < new_closest_dist) {
				new_closest = i;
				new_closest_dist = d;
			}
		}

		if (new_closest != cur_closest && new_closest != -1) {
			if (blend_mode == BLEND_MODE_DISCRETE_CARRY && cur_closest != -1) {
				Ref<AnimationNodeAnimation> na_c = static_cast<Ref<AnimationNodeAnimation>>(blend_points[cur_closest].node);
				Ref<AnimationNodeAnimation> na_n = static_cast<Ref<AnimationNodeAnimation>>(blend_points[new_closest].node);
				if (na_c.is_valid() && na_n.is_valid()) {
					na_n->process_state = process_state;
					na_c->process_state = process_state;
					na_n->set_backward(na_c->is_backward());
					na_n = nullptr;
					na_c = nullptr;
				}

				AnimationMixer::PlaybackInfo pi_carry = pi;
				pi_carry.seeked = false;
				pi_carry.weight = 0.0;

				NodeTimeInfo from = blend_node(blend_points[cur_closest].node, blend_points[cur_closest].name, pi_carry, FILTER_IGNORE, true, true);
				pi.time = from.position;
			}

			pi.seeked = true;
			cur_closest = new_closest;
		}

		if (new_closest != -1) {
			weights[new_closest] = 1.0f;
		}
	}

	// Master index (largest weight).
	for (int i = 0; i < blend_points_used; i++) {
		if (weights[i] > max_weight) {
			max_weight = weights[i];
			master_idx = i;
		}
	}

	// --- Sync mode flags and mode-change detection ---
	const bool sync_none = (sync_mode == SYNC_MODE_NONE);
	const bool sync_abs = (sync_mode == SYNC_MODE_ABSOLUTE);
	const bool sync_norm = (sync_mode == SYNC_MODE_NORMALIZED);

	bool mode_changed_this_frame = false;
	if (pending_resync) {
		mode_changed_this_frame = true;
		pending_resync = false;
	}

	AnimationMixer *mixer = process_state ? process_state->tree : nullptr;

	// Normalized sync state (only meaningful if sync_norm).
	AnimationNodeUtils::NormalizedSyncState norm_state = AnimationNodeUtils::prepare_normalized_sync_state(
		sync_norm,                // p_is_normalized
		mode_changed_this_frame,  // p_force_resync
		weights,
		blend_points_used,
		master_idx,
		process_state,
		pi,
		this,
		_blend_space_1d_get_length,
		_blend_space_1d_peek
	);

	// --- Execution ---
	for (int i = 0; i < blend_points_used; i++) {
		double w = weights[i];
		const bool is_active = (w > CMP_EPSILON);

		if (!is_active && sync_none) {
			// In NONE mode, completely skip zero-weight clips.
			continue;
		}

		AnimationMixer::PlaybackInfo pi_child = pi;
		pi_child.weight = w;

		if (sync_norm && norm_state.enabled) {
			const double child_len = _blend_space_1d_get_length(this, i, mixer);
			AnimationNodeUtils::apply_normalized_sync_to_child(norm_state, pi_child, pi, child_len);
		} else if (sync_abs) {
			// SYNC_MODE_ABSOLUTE:
			// - On mode change, snap all children to the current absolute time once.
			// - On later external seeks, propagate that absolute time as well.
			if (mode_changed_this_frame || pi.seeked) {
				pi_child.seeked = true;
				pi_child.time = pi.time;
			}
		} else {
			// SYNC_MODE_NONE
		}

		NodeTimeInfo t = blend_node(blend_points[i].node, blend_points[i].name, pi_child, FILTER_IGNORE, true, p_test_only);
		if (i == master_idx) {
			mind = t;
		} else if (mind.length == 0.0) {
			mind = t;
		}
	}

	set_parameter(closest, cur_closest);
	return mind;
}

String AnimationNodeBlendSpace1D::get_caption() const {
	return "BlendSpace1D";
}

AnimationNodeBlendSpace1D::AnimationNodeBlendSpace1D() {
	for (int i = 0; i < MAX_BLEND_POINTS; i++) {
		blend_points[i].name = itos(i);
	}

	last_sync_mode = sync_mode;
	pending_resync = false;
}

AnimationNodeBlendSpace1D::~AnimationNodeBlendSpace1D() {
}
