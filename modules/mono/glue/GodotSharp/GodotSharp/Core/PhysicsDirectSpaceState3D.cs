using System;
using Godot.NativeInterop;

#nullable enable

namespace Godot
{
    public partial class PhysicsDirectSpaceState3D
    {
        public unsafe bool TryIntersectRay(PhysicsRayQueryParameters3D parameters, out PhysicsRayQueryResult3D result)
        {
            result = default;
            return NativeFuncs.godotsharp_physics_direct_space_state_3d_intersect_ray(
                GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), out result).ToBool();
        }

        public unsafe int IntersectRayAll(PhysicsRayQueryParameters3D parameters, Span<PhysicsRayQueryResult3D> results)
        {
            if (results.IsEmpty)
                return 0;

            fixed (PhysicsRayQueryResult3D* resultsPtr = results)
            {
                return NativeFuncs.godotsharp_physics_direct_space_state_3d_intersect_ray_all(
                    GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), resultsPtr, results.Length);
            }
        }

        public unsafe int IntersectRays(PhysicsRayQueryParameters3D parameters, ReadOnlySpan<PhysicsRayCommand3D> commands, Span<PhysicsRayQueryResult3D> results)
        {
            if (results.Length < commands.Length)
                throw new ArgumentException("Results span must be at least as long as commands span.", nameof(results));

            if (commands.IsEmpty)
                return 0;

            fixed (PhysicsRayCommand3D* commandsPtr = commands)
            fixed (PhysicsRayQueryResult3D* resultsPtr = results)
            {
                return NativeFuncs.godotsharp_physics_direct_space_state_3d_intersect_ray_batch(
                    GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), commandsPtr, commands.Length, resultsPtr);
            }
        }

        public unsafe int IntersectRaysAll(PhysicsRayQueryParameters3D parameters, ReadOnlySpan<PhysicsRayCommand3D> commands, int maxHitsPerRay, Span<int> hitCounts, Span<PhysicsRayQueryResult3D> results)
        {
            ArgumentOutOfRangeException.ThrowIfNegative(maxHitsPerRay);

            if (hitCounts.Length < commands.Length)
                throw new ArgumentException("Hit counts span must be at least as long as commands span.", nameof(hitCounts));

            if ((long)results.Length < (long)commands.Length * maxHitsPerRay)
                throw new ArgumentException("Results span must be at least commands.Length * maxHitsPerRay.", nameof(results));

            if (commands.IsEmpty)
                return 0;

            if (maxHitsPerRay == 0)
            {
                hitCounts[..commands.Length].Clear();
                return 0;
            }

            fixed (PhysicsRayCommand3D* commandsPtr = commands)
            fixed (int* hitCountsPtr = hitCounts)
            fixed (PhysicsRayQueryResult3D* resultsPtr = results)
            {
                return NativeFuncs.godotsharp_physics_direct_space_state_3d_intersect_ray_batch_all(
                    GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), commandsPtr, commands.Length, resultsPtr, maxHitsPerRay, hitCountsPtr);
            }
        }

        public unsafe int IntersectPoint(PhysicsPointQueryParameters3D parameters, Span<PhysicsShapeQueryResult3D> results)
        {
            fixed (PhysicsShapeQueryResult3D* resultsPtr = results)
            {
                return NativeFuncs.godotsharp_physics_direct_space_state_3d_intersect_point(
                    GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), resultsPtr, results.Length);
            }
        }

        public unsafe int IntersectShape(PhysicsShapeQueryParameters3D parameters, Span<PhysicsShapeQueryResult3D> results)
        {
            fixed (PhysicsShapeQueryResult3D* resultsPtr = results)
            {
                return NativeFuncs.godotsharp_physics_direct_space_state_3d_intersect_shape(
                    GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), resultsPtr, results.Length);
            }
        }

        public bool CastMotion(PhysicsShapeQueryParameters3D parameters, out real_t closestSafe, out real_t closestUnsafe)
        {
            return NativeFuncs.godotsharp_physics_direct_space_state_3d_cast_motion(
                GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), out closestSafe, out closestUnsafe).ToBool();
        }

        public unsafe int CollideShape(PhysicsShapeQueryParameters3D parameters, Span<PhysicsShapeCollision3D> results)
        {
            fixed (PhysicsShapeCollision3D* resultsPtr = results)
            {
                return NativeFuncs.godotsharp_physics_direct_space_state_3d_collide_shape(
                    GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), resultsPtr, results.Length);
            }
        }

        public bool TryGetRestInfo(PhysicsShapeQueryParameters3D parameters, out PhysicsShapeRestInfo3D result)
        {
            result = default;
            return NativeFuncs.godotsharp_physics_direct_space_state_3d_rest_info(
                GodotObject.GetPtr(this), GodotObject.GetPtr(parameters), out result).ToBool();
        }
    }
}
