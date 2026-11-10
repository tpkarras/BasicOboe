package org.tpkarras.basicoboe.constants;

public final class PerformanceMode {
    private PerformanceMode(){
        throw new UnsupportedOperationException("Not a instantiable class");
    }

    public static final String LowLatency = "LowLatency";
    public static final String PowerSaving = "PowerSaving";
    public static final String PowerSaving_Offloaded = "PowerSaving_Offloaded";
}
