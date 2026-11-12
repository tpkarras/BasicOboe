package org.tpkarras.basicoboe.constants;
public final class Usage {

    private Usage(){
        throw new UnsupportedOperationException("Not a instantiable class");
    }

    public static final int Media = 1;
    public static final int VoiceCommunication = 2;
    public static final int VoiceCommunicationSignalling = 3;
    public static final int Alarm = 4;
    public static final int Notification = 5;
    public static final int NotificationRingtone = 6;
    public static final int NotificationEvent = 10;
    public static final int AssistanceAccessibility = 11;
    public static final int AssistanceNavigationGuidance = 12;
    public static final int AssistanceSonification = 13;
    public static final int Game = 14;
    public static final int Assistant = 16;
}
