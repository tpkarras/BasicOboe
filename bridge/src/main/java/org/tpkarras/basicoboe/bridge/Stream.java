package org.tpkarras.basicoboe.bridge;

import android.content.Context;
import android.media.AudioDeviceInfo;
import android.os.Build;
import android.util.Log;

import androidx.annotation.NonNull;

import org.tpkarras.basicoboe.bridge.constants.AudioAPIs;
import org.tpkarras.basicoboe.bridge.constants.AudioDirection;
import org.tpkarras.basicoboe.bridge.constants.PerformanceMode;
import org.tpkarras.basicoboe.bridge.constants.SampleRate;
import org.tpkarras.basicoboe.bridge.constants.Usage;

import java.util.Objects;

public final class Stream {

    static {
        Log.d("BasicOboe", "Loading bridge library");
        System.loadLibrary("bridge");
    }
    private final AudioDeviceInfo device;
    private boolean initialized = false;
    private String Format;
    private int iSampleRate;
    private int ChannelCount;
    private int ChannelMask = 0;
    private int Direction = 0;
    private String AudioAPI;
    private String performanceMode;
    private boolean sharingMode = false;
    private int famesPerCallback = 0;
    private boolean IsSpatial = false;
    private String iUsage;
    private native void deviceInitialize(String packageName, String attributionTag, int deviceID, String format, int sampleRate, int channels, int channelMask, String audioAPI, String performanceMode, int direction, boolean sharingMode, int famesPerCallback, boolean isSpatial, String usage);
    private native void deviceTerminate();
    private native float[] readInput();

    private native void writeOutput(float[] output);

    private boolean ObjectMatch(@NonNull Object check, @NonNull Object[] checkAgainst) throws Exception {

        if(check.getClass() != checkAgainst.getClass().getComponentType()){
            throw new Exception("Objects must be the same class");
        }

        boolean valid = false;

        for (Object current : checkAgainst) {

            if (check.equals(current)) {

                valid = true;
                break;

            }

        }

        return valid;

    }

    public Stream(@NonNull AudioDeviceInfo Device){

        if(Device.isSource()){
            Direction = AudioDirection.Input;
        }

        device = Device;

    }

    public void terminate() throws Exception {
        if(!initialized){
            throw new Exception("Audio device already terminated");
        }

        deviceTerminate();
        initialized = false;
    }

    public void initialize(@NonNull Context context) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        String format;
        int sampleRate;
        int channels;
        String audioAPI;
        String usage;
        String modePerformance;
        String attributionTag;

        modePerformance = Objects.requireNonNullElse(performanceMode, "");
        format  = Objects.requireNonNullElse(Format, "");
        usage  = Objects.requireNonNullElse(iUsage, "");
        audioAPI = Objects.requireNonNullElse(AudioAPI, "");

        if(iSampleRate <= 0){
            sampleRate = 0;
        } else {
            sampleRate = iSampleRate;
        }

        if(ChannelCount <= 0){
            channels = 0;
        } else {
            channels = ChannelCount;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            attributionTag = context.getAttributionTag();
        } else {
            attributionTag = "";
        }

        deviceInitialize(context.getPackageName(), attributionTag, device.getId(), format, sampleRate, channels, ChannelMask, audioAPI, modePerformance, Direction, sharingMode, famesPerCallback, IsSpatial, usage);
    initialized = true;
    }

    public float[] getInput() throws Exception {

        if(!initialized){
            throw new Exception("Audio device not initialized");
        }

        if(Direction != AudioDirection.Input && Direction != AudioDirection.Duplex){
            throw new Exception("Stream is not a input");
        }

        return readInput();
    }

    public void sendOutput(float[] samples) throws Exception {

        if(!initialized){
            throw new Exception("Audio device not initialized");
        }

        if(Direction != AudioDirection.Output && Direction != AudioDirection.Duplex){
            throw new Exception("Stream is not a output");
        }

        writeOutput(samples);

    }

    public void setFormat(@NonNull String format) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(!ObjectMatch(format, new String[]{org.tpkarras.basicoboe.bridge.constants.Format.I16, org.tpkarras.basicoboe.bridge.constants.Format.Float, org.tpkarras.basicoboe.bridge.constants.Format.I24, org.tpkarras.basicoboe.bridge.constants.Format.I32, org.tpkarras.basicoboe.bridge.constants.Format.IEC61937, org.tpkarras.basicoboe.bridge.constants.Format.MP3, org.tpkarras.basicoboe.bridge.constants.Format.AAC_LC, org.tpkarras.basicoboe.bridge.constants.Format.AAC_HE_V1, org.tpkarras.basicoboe.bridge.constants.Format.AAC_HE_V2, org.tpkarras.basicoboe.bridge.constants.Format.AAC_ELD, org.tpkarras.basicoboe.bridge.constants.Format.AAC_XHE, org.tpkarras.basicoboe.bridge.constants.Format.OPUS})){
            throw new Exception("Not a valid audio API");
        }

        Format = format;

    }

    public void setDirection(int direction) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(!ObjectMatch((Integer)direction, new Integer[]{AudioDirection.Input, AudioDirection.Output, AudioDirection.Duplex})){
            throw new Exception("Not a valid direction");
        }

        if(direction == AudioDirection.Duplex && !(device.isSource() && device.isSink())) {
            throw new Exception("Audio device is not a duplex device");
        }

        if(direction == AudioDirection.Input && !device.isSource()) {
            throw new Exception("Audio device is not a input device");
        }

        if(direction == AudioDirection.Output && !device.isSink()) {
            throw new Exception("Audio device is not a output device");
        }

        Direction = direction;

    }

    public void setSampleRate(int sampleRate) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        boolean valid = false;

        for (int check : new int[]{SampleRate.KHz8, SampleRate.KHz11p25, SampleRate.KHz16, SampleRate.KHz22p5, SampleRate.KHz44p1, SampleRate.KHz48, SampleRate.KHz88p2, SampleRate.KHz96, SampleRate.KHz176p4, SampleRate.KHz192}) {

            if (check == sampleRate) {

                valid = true;
                break;

            }

        }

        if(!valid){
            throw new Exception("Not a valid sample rate");
        }

        iSampleRate = sampleRate;

    }

    public void setChannelCount(int channels) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(channels < 0){
            throw new Exception("Invalid channel count");
        }

        ChannelCount = channels;

    }

    public void setChannelMask(int[] channelMasks) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(channelMasks.length == 1 && channelMasks[0] == org.tpkarras.basicoboe.bridge.constants.ChannelMask.Stereo) {

            ChannelMask = channelMasks[0];

        } else {

            int[] masks = {org.tpkarras.basicoboe.bridge.constants.ChannelMask.Unspecified, org.tpkarras.basicoboe.bridge.constants.ChannelMask.FrontLeft, org.tpkarras.basicoboe.bridge.constants.ChannelMask.FrontRight, org.tpkarras.basicoboe.bridge.constants.ChannelMask.FrontCenter, org.tpkarras.basicoboe.bridge.constants.ChannelMask.LowFrequency, org.tpkarras.basicoboe.bridge.constants.ChannelMask.BackLeft, org.tpkarras.basicoboe.bridge.constants.ChannelMask.BackLeft, org.tpkarras.basicoboe.bridge.constants.ChannelMask.BackRight, org.tpkarras.basicoboe.bridge.constants.ChannelMask.FrontLeftOfCenter, org.tpkarras.basicoboe.bridge.constants.ChannelMask.FrontRightOfCenter, org.tpkarras.basicoboe.bridge.constants.ChannelMask.BackCenter, org.tpkarras.basicoboe.bridge.constants.ChannelMask.SideLeft, org.tpkarras.basicoboe.bridge.constants.ChannelMask.SideRight};
            int tempChannelMask = 0;

            for (int channelMask : channelMasks) {

                for (int mask : masks) {

                    if (mask != channelMask) {

                    throw new Exception("Not a valid channel mask");

                    }

                }

                tempChannelMask |= channelMask;

            }

            ChannelMask = tempChannelMask;

        }

    }

    public void setAudioAPI(@NonNull String audioAPI) throws Exception{

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(!ObjectMatch(audioAPI, new String[]{AudioAPIs.AAudio, AudioAPIs.OpenSL_ES, AudioAPIs.Unspecified})){
            throw new Exception("Not a valid audio API");
        }

        AudioAPI = audioAPI;

    }

    public void setPerformanceMode(@NonNull String mode) throws Exception{

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(!ObjectMatch(mode, new String[]{PerformanceMode.LowLatency, PerformanceMode.PowerSaving, PerformanceMode.PowerSaving_Offloaded})){
            throw new Exception("Not a valid performance mode");
        }

        performanceMode = mode;

    }

    public void setSharingMode(boolean mode) throws Exception{

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        sharingMode = mode;

    }

    public void setFramesPerCallback(int frames) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(frames < 0){
            throw new Exception("Frames cannot be less than 0");
        }

        famesPerCallback = frames;

    }

    public void setSpatialization(boolean isSpatial) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        IsSpatial = isSpatial;

    }

    public void setUsage(String usage) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(!ObjectMatch(usage, new String[]{Usage.Alarm, Usage.AssistanceAccessibility, Usage.VoiceCommunicationSignalling, Usage.Notification, Usage.AssistanceNavigationGuidance, Usage.Assistant, Usage.AssistanceSonification, Usage.Game, Usage.Media, Usage.NotificationRingtone, Usage.VoiceCommunication, Usage.NotificationEvent})){
            throw new Exception("Not a valid audio API");
        }

        iUsage = usage;

    }

}
