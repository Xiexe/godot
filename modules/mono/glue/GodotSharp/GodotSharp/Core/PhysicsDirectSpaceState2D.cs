using System;
using Godot.NativeInterop;

#nullable enable

namespace Godot
{
    public partial class PhysicsDirectSpaceState2D
    {
        public unsafe bool TryIntersectRay(PhysicsRayQueryParameters2D parameters, out PhysicsRayQueryResult2D result)
        {
            result = default;
            return NativeFuncs.godotsharp_physics_direct_space_state_2d_intersect_ray(
                GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), out result).ToBool();
        }

        public unsafe int IntersectPoint(PhysicsPointQueryParameters2D parameters, Span<PhysicsShapeQueryResult2D> results)
        {
            fixed (PhysicsShapeQueryResult2D* resultsPtr = results)
            {
                return NativeFuncs.godotsharp_physics_direct_space_state_2d_intersect_point(
                    GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), resultsPtr, results.Length);
            }
        }

        public unsafe int IntersectShape(PhysicsShapeQueryParameters2D parameters, Span<PhysicsShapeQueryResult2D> results)
        {
            fixed (PhysicsShapeQueryResult2D* resultsPtr = results)
            {
                return NativeFuncs.godotsharp_physics_direct_space_state_2d_intersect_shape(
                    GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), resultsPtr, results.Length);
            }
        }

        public bool CastMotion(PhysicsShapeQueryParameters2D parameters, out real_t closestSafe, out real_t closestUnsafe)
        {
            return NativeFuncs.godotsharp_physics_direct_space_state_2d_cast_motion(
                GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), out closestSafe, out closestUnsafe).ToBool();
        }

        public unsafe int CollideShape(PhysicsShapeQueryParameters2D parameters, Span<PhysicsShapeCollision2D> results)
        {
            fixed (PhysicsShapeCollision2D* resultsPtr = results)
            {
                return NativeFuncs.godotsharp_physics_direct_space_state_2d_collide_shape(
                    GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), resultsPtr, results.Length);
            }
        }

        public bool TryGetRestInfo(PhysicsShapeQueryParameters2D parameters, out PhysicsShapeRestInfo2D result)
        {
            result = default;
            return NativeFuncs.godotsharp_physics_direct_space_state_2d_rest_info(
                GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), out result).ToBool();
        }
    }
}
