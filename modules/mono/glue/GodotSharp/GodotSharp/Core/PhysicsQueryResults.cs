using System.Runtime.InteropServices;

#nullable enable

namespace Godot
{
    [StructLayout(LayoutKind.Sequential)]
    public struct PhysicsRayQueryResult2D
    {
        public Vector2 Position;
        public Vector2 Normal;
        public Rid Rid;
        public ulong ColliderId;
        public int Shape;

        public readonly GodotObject? GetCollider() => ColliderId == 0 ? null : GodotObject.InstanceFromId(ColliderId);

        public readonly T? GetCollider<T>() where T : GodotObject => GetCollider() as T;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PhysicsRayQueryResult3D
    {
        public Vector3 Position;
        public Vector3 Normal;
        public Rid Rid;
        public ulong ColliderId;
        public int Shape;
        public int FaceIndex;

        public readonly GodotObject? GetCollider() => ColliderId == 0 ? null : GodotObject.InstanceFromId(ColliderId);

        public readonly T? GetCollider<T>() where T : GodotObject => GetCollider() as T;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PhysicsRayCommand3D
    {
        public Vector3 From;
        public Vector3 To;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PhysicsShapeQueryResult2D
    {
        public Rid Rid;
        public ulong ColliderId;
        public int Shape;

        public readonly GodotObject? GetCollider() => ColliderId == 0 ? null : GodotObject.InstanceFromId(ColliderId);

        public readonly T? GetCollider<T>() where T : GodotObject => GetCollider() as T;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PhysicsShapeQueryResult3D
    {
        public Rid Rid;
        public ulong ColliderId;
        public int Shape;

        public readonly GodotObject? GetCollider() => ColliderId == 0 ? null : GodotObject.InstanceFromId(ColliderId);

        public readonly T? GetCollider<T>() where T : GodotObject => GetCollider() as T;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PhysicsShapeRestInfo2D
    {
        public Vector2 Point;
        public Vector2 Normal;
        public Rid Rid;
        public ulong ColliderId;
        public int Shape;
        public Vector2 LinearVelocity;

        public readonly GodotObject? GetCollider() => ColliderId == 0 ? null : GodotObject.InstanceFromId(ColliderId);

        public readonly T? GetCollider<T>() where T : GodotObject => GetCollider() as T;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PhysicsShapeRestInfo3D
    {
        public Vector3 Point;
        public Vector3 Normal;
        public Rid Rid;
        public ulong ColliderId;
        public int Shape;
        public Vector3 LinearVelocity;

        public readonly GodotObject? GetCollider() => ColliderId == 0 ? null : GodotObject.InstanceFromId(ColliderId);

        public readonly T? GetCollider<T>() where T : GodotObject => GetCollider() as T;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PhysicsShapeCollision2D
    {
        public Vector2 QueryPoint;
        public Vector2 ColliderPoint;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PhysicsShapeCollision3D
    {
        public Vector3 QueryPoint;
        public Vector3 ColliderPoint;
    }
}
