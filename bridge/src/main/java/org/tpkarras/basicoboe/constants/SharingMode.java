package org.tpkarras.basicoboe.constants;

public final class SharingMode {

    private SharingMode(){
        throw new UnsupportedOperationException("Not a instantiable class");
    }

    public static final boolean Exclusive = true;
    public static final boolean Shared = false;

}
