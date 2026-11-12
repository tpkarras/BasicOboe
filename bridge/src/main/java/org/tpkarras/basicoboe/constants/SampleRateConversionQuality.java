package org.tpkarras.basicoboe.constants;

public final class SampleRateConversionQuality {

    private SampleRateConversionQuality() {
        throw new UnsupportedOperationException("Not a instantiable class");
    }

    public static final int None = 0;
    public static final int Fastest = 1;
    public static final int Low = 2;
    public static final int Medium = 3;
    public static final int High = 4;
    public static final int Best = 5;

}
