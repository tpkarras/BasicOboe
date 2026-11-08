package org.tpkarras.basicoboe.bridge.constants;

public final class ChannelMask {

    private ChannelMask(){
        throw new UnsupportedOperationException("Not a instantiable class");
    }
    public static final int Unspecified = 0;
    public static final int FrontLeft = 1;
    public static final int FrontRight = 1 << 1;
    public static final int FrontCenter = 1 << 2;
    public static final int LowFrequency = 1 << 3;
    public static final int BackLeft = 1 << 4;
    public static final int BackRight = 1 << 5;
    public static final int FrontLeftOfCenter = 1 << 6;
    public static final int FrontRightOfCenter = 1 << 7;
    public static final int BackCenter = 1 << 8;
    public static final int SideLeft = 1 << 9;
    public static final int SideRight = 1 << 10;
    public static final int Stereo = (1) | (1 << 1);
}
