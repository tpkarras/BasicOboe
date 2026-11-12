package org.tpkarras.basicoboe.constants;

public final class AudioAPIs {

    private AudioAPIs(){
        throw new UnsupportedOperationException("Not a instantiable class");
    }

    public static final int AAudio = 2;
    public static final int OpenSL_ES = 1;
    public static final int Unspecified = 0;

}