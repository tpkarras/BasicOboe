package org.tpkarras.basicoboe.bridge.constants;

public final class AudioDirection {
    private AudioDirection(){
        throw new UnsupportedOperationException("Not a instantiable class");
    }

    public static final int Output = 0;
    public static final int Input = 1;
    public static final int Duplex = 2;
}
