namespace Godot
{

using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Godot.NativeInterop;

public interface IInputEventMouseMotionHandler
{
    void OnInput(in InputEventMouseMotionData eventData);
}

public interface IInputEventMouseButtonHandler
{
    void OnInput(in InputEventMouseButtonData eventData);
}

public interface IInputEventKeyHandler
{
    void OnInput(in InputEventKeyData eventData);
}

[StructLayout(LayoutKind.Sequential)]
public readonly struct InputEventMouseMotionData
{
    public readonly int Device;
    public readonly long WindowId;
    public readonly MouseButtonMask ButtonMask;
    public readonly Vector2 Position;
    public readonly Vector2 GlobalPosition;
    public readonly Vector2 Relative;
    public readonly Vector2 ScreenRelative;
    public readonly Vector2 Velocity;
    public readonly Vector2 ScreenVelocity;
    public readonly Vector2 Tilt;
    public readonly float Pressure;
    private readonly godot_bool _canceled;
    private readonly godot_bool _shiftPressed;
    private readonly godot_bool _altPressed;
    private readonly godot_bool _ctrlPressed;
    private readonly godot_bool _metaPressed;
    private readonly godot_bool _commandOrControlAutoremap;
    private readonly godot_bool _penInverted;

    public bool Canceled
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _canceled.ToBool();
    }

    public bool ShiftPressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _shiftPressed.ToBool();
    }

    public bool AltPressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _altPressed.ToBool();
    }

    public bool CtrlPressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _ctrlPressed.ToBool();
    }

    public bool MetaPressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _metaPressed.ToBool();
    }

    public bool CommandOrControlAutoremap
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _commandOrControlAutoremap.ToBool();
    }

    public bool PenInverted
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _penInverted.ToBool();
    }
}

[StructLayout(LayoutKind.Sequential)]
public readonly struct InputEventMouseButtonData
{
    public readonly int Device;
    public readonly long WindowId;
    public readonly MouseButtonMask ButtonMask;
    public readonly Vector2 Position;
    public readonly Vector2 GlobalPosition;
    public readonly float Factor;
    public readonly MouseButton ButtonIndex;
    private readonly godot_bool _canceled;
    private readonly godot_bool _pressed;
    private readonly godot_bool _doubleClick;
    private readonly godot_bool _shiftPressed;
    private readonly godot_bool _altPressed;
    private readonly godot_bool _ctrlPressed;
    private readonly godot_bool _metaPressed;
    private readonly godot_bool _commandOrControlAutoremap;

    public bool Canceled
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _canceled.ToBool();
    }

    public bool Pressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _pressed.ToBool();
    }

    public bool DoubleClick
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _doubleClick.ToBool();
    }

    public bool ShiftPressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _shiftPressed.ToBool();
    }

    public bool AltPressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _altPressed.ToBool();
    }

    public bool CtrlPressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _ctrlPressed.ToBool();
    }

    public bool MetaPressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _metaPressed.ToBool();
    }

    public bool CommandOrControlAutoremap
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _commandOrControlAutoremap.ToBool();
    }
}

[StructLayout(LayoutKind.Sequential)]
public readonly struct InputEventKeyData
{
    public readonly int Device;
    public readonly long WindowId;
    public readonly Key Keycode;
    public readonly Key PhysicalKeycode;
    public readonly Key KeyLabel;
    private readonly uint _unicode;
    public readonly KeyLocation Location;
    private readonly godot_bool _canceled;
    private readonly godot_bool _pressed;
    private readonly godot_bool _echo;
    private readonly godot_bool _shiftPressed;
    private readonly godot_bool _altPressed;
    private readonly godot_bool _ctrlPressed;
    private readonly godot_bool _metaPressed;
    private readonly godot_bool _commandOrControlAutoremap;

    public long Unicode
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _unicode;
    }

    public bool Canceled
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _canceled.ToBool();
    }

    public bool Pressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _pressed.ToBool();
    }

    public bool Echo
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _echo.ToBool();
    }

    public bool ShiftPressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _shiftPressed.ToBool();
    }

    public bool AltPressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _altPressed.ToBool();
    }

    public bool CtrlPressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _ctrlPressed.ToBool();
    }

    public bool MetaPressed
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _metaPressed.ToBool();
    }

    public bool CommandOrControlAutoremap
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        get => _commandOrControlAutoremap.ToBool();
    }
}

}

namespace Godot.Bridge
{

using System;
using Godot.NativeInterop;

internal static class FastInputDispatch
{
    private enum InputEventFastType
    {
        None = 0,
        MouseMotion = 1,
        MouseButton = 2,
        Key = 3,
    }

    internal static bool HasInputHandler(GodotObject godotObject)
        => godotObject is IInputEventMouseMotionHandler ||
           godotObject is IInputEventMouseButtonHandler ||
           godotObject is IInputEventKeyHandler;

    internal static bool TryInvokeInput(GodotObject godotObject, NativeVariantPtrArgs args, out godot_variant ret)
    {
        ret = default;

        if (!HasInputHandler(godotObject) || args.Count != 1)
        {
            return false;
        }

        IntPtr inputEvent = VariantUtils.ConvertToGodotObjectPtr(args[0]);
        if (inputEvent == IntPtr.Zero)
        {
            return false;
        }

        switch ((InputEventFastType)NativeFuncs.godotsharp_input_event_get_fast_type(inputEvent))
        {
            case InputEventFastType.MouseMotion:
            {
                if (godotObject is not IInputEventMouseMotionHandler handler)
                {
                    return false;
                }

                NativeFuncs.godotsharp_input_event_as_mouse_motion(inputEvent, out InputEventMouseMotionData eventData);
                handler.OnInput(in eventData);
                return true;
            }
            case InputEventFastType.MouseButton:
            {
                if (godotObject is not IInputEventMouseButtonHandler handler)
                {
                    return false;
                }

                NativeFuncs.godotsharp_input_event_as_mouse_button(inputEvent, out InputEventMouseButtonData eventData);
                handler.OnInput(in eventData);
                return true;
            }
            case InputEventFastType.Key:
            {
                if (godotObject is not IInputEventKeyHandler handler)
                {
                    return false;
                }

                NativeFuncs.godotsharp_input_event_as_key(inputEvent, out InputEventKeyData eventData);
                handler.OnInput(in eventData);
                return true;
            }
            default:
                return false;
        }
    }
}
}
