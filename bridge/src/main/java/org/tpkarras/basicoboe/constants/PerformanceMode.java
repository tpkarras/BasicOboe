package org.tpkarras.basicoboe.constants;

public final class PerformanceMode {
    private PerformanceMode(){
        throw new UnsupportedOperationException("Not a instantiable class");
    }
    public static final int None = 10;
    public static final int LowLatency = 11;
    public static final int PowerSaving = 12;
    public static final int PowerSavingOffloaded = 13;
}
