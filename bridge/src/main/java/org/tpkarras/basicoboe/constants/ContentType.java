package org.tpkarras.basicoboe.constants;

public final class ContentType {
    private ContentType(){
        throw new UnsupportedOperationException("Not a instantiable class");
    }
    public static final int Speech = 1;
    public static final int Music = 2;
    public static final int Movie = 3;
    public static final int Sonification = 4;

}
