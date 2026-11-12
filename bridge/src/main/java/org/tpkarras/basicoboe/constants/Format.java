package org.tpkarras.basicoboe.constants;

public final class Format {

    private Format(){
        throw new UnsupportedOperationException("Not a instantiable class");
    }
public static final int Invalid = -1;
    public static final int Unspecified = 0;
    public static final int I16 = 1;
    public static final int Float = 2;
    public static final int I24 = 3;
    public static final int I32 = 4;
    public static final int IEC61937 = 5;
    public static final int MP3 = 6;
    public static final int AAC_LC = 7;
    public static final int AAC_HE_V1 = 8;
    public static final int AAC_HE_V2 = 9;
    public static final int AAC_ELD = 10;
    public static final int AAC_XHE = 11;
    public static final int OPUS = 12;
}
