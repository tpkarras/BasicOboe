package org.tpkarras.basicoboe;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.media.AudioDeviceInfo;
import android.os.Build;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.RequiresPermission;
import androidx.core.content.ContextCompat;

import org.tpkarras.basicoboe.constants.AudioAPIs;
import org.tpkarras.basicoboe.constants.ContentType;
import org.tpkarras.basicoboe.constants.Format;
import org.tpkarras.basicoboe.constants.PerformanceMode;
import org.tpkarras.basicoboe.constants.SampleRate;
import org.tpkarras.basicoboe.constants.SampleRateConversionQuality;
import org.tpkarras.basicoboe.constants.Usage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Objects;

public final class Stream {

    static {
        Log.d("BasicOboe", "Loading bridge library");
        System.loadLibrary("bridge");
    }
    private final int inputDevice;
    private final int outputDevice;
    private boolean initialized = false;
    private int inputFormat;
    private int outputFormat;
    private int InputSampleRate;
    private int OutputSampleRate;
    private int InputChannelCount;
    private int OutputChannelCount;
    private int InputChannelMask;
    private int OutputChannelMask;
    private int AudioAPI;
    private int performanceMode;
    private boolean inputSharingMode;
    private boolean outputSharingMode;
    private int framesPerCallback;
    private boolean InputIsSpatial;
    private boolean OutputIsSpatial;
    private int InputUsage;
    private int OutputUsage;
    private int InputContentType;
    private int OutputContentType;
    private int iSampleRateConversionQuality;
    private native void deviceInitialize(String packageName, String attributionTag, int deviceIDInput, int deviceIDOutput, int inputFormat, int outputFormat, int inputSampleRate, int outputSampleRate, int inputChannels, int outputChannels, int inputChannelMask, int outputChannelMask, int audioAPI, int performanceMode, boolean inputSharingMode, boolean outputSharingMode, int famesPerCallback, boolean inputIsSpatial, boolean outputIsSpatial, int inputUsage, int outputUsage, int inputContentType, int outputContentType, int SampleRateConversionQuality);
    private native void deviceTerminate();
    private native ByteBuffer readInput();
    private native int[] getSampleRate();
    private native int[] getChannelCount();
    private native int[] getDeviceId();
    private native int[] getFormat();
    private native int[] getContentType();

    private native int[] getChannelMask();

    private native boolean[] getSharingMode();

    private native void writeOutput(ByteBuffer output);

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

    public Stream(@Nullable AudioDeviceInfo InputDevice, @Nullable AudioDeviceInfo OutputDevice) throws Exception {

        if(InputDevice == null && OutputDevice == null){
            throw new Exception("Both parameters cannot be null");
        }

        if(InputDevice != null) {

            if (!InputDevice.isSource()) {
                throw new Exception("Audio device not an input");
            }

            inputDevice = InputDevice.getId();
            Log.d("InputDevice", String.valueOf(inputDevice));

        } else {
            inputDevice = 0;
        }

        if(OutputDevice != null) {

            if (!OutputDevice.isSink()) {
                throw new Exception("Audio device not an output");
            }

            outputDevice = OutputDevice.getId();

        } else {
            outputDevice = 0;
        }

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

        if(inputDevice > 0 && (ContextCompat.checkSelfPermission(context, Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_DENIED)){
            throw new Exception("Cannot open input device, RECORD_AUDIO permission not granted");
        }

        int formatInput;
        int formatOutput;
        int inputSampleRate;
        int outputSampleRate;
        int inputChannelCount;
        int outputChannelCount;
        int audioAPI;
        int inputUsage;
        int outputUsage;
        int inputContentType;
        int outputContentType;
        int modePerformance;
        int sampleRateConversionQuality;
        boolean iinputSharingMode;
        boolean ioutputSharingMode;
        boolean iInputIsSpatial;
        boolean iOutputIsSpatial;
        int iInputChannelMask;
        int iOutputChannelMask;
        int iFramesPerCallback;
        String attributionTag;

        modePerformance = Objects.requireNonNullElse(performanceMode, 0);
        sampleRateConversionQuality = Objects.requireNonNullElse(iSampleRateConversionQuality, 0);
        formatInput  = Objects.requireNonNullElse(inputFormat, 0);
        formatOutput  = Objects.requireNonNullElse(outputFormat, 0);
        inputContentType  = Objects.requireNonNullElse(InputContentType, 0);
        outputContentType  = Objects.requireNonNullElse(OutputContentType, 0);
        outputUsage  = Objects.requireNonNullElse(OutputUsage, 0);
        inputUsage  = Objects.requireNonNullElse(InputUsage, 0);
        audioAPI = Objects.requireNonNullElse(AudioAPI, 0);
        inputSampleRate = Objects.requireNonNullElse(InputSampleRate, 0);
        outputSampleRate = Objects.requireNonNullElse(OutputSampleRate, 0);
        inputChannelCount = Objects.requireNonNullElse(InputChannelCount, 0);
        outputChannelCount = Objects.requireNonNullElse(OutputChannelCount, 0);
        iinputSharingMode = Objects.requireNonNullElse(inputSharingMode, false);
        ioutputSharingMode = Objects.requireNonNullElse(outputSharingMode, false);
        iInputIsSpatial = Objects.requireNonNullElse(InputIsSpatial, false);
        iOutputIsSpatial = Objects.requireNonNullElse(OutputIsSpatial, false);
        iInputChannelMask = Objects.requireNonNullElse(InputChannelMask, 0);
        iOutputChannelMask = Objects.requireNonNullElse(OutputChannelMask, 0);
        iFramesPerCallback = Objects.requireNonNullElse(framesPerCallback, 0);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            attributionTag = context.getAttributionTag();
        } else {
            attributionTag = "";
        }

        deviceInitialize(context.getPackageName(), attributionTag, inputDevice, outputDevice, formatInput, formatOutput, inputSampleRate, outputSampleRate, inputChannelCount, outputChannelCount, iInputChannelMask, iOutputChannelMask, audioAPI, modePerformance, iinputSharingMode, ioutputSharingMode, iFramesPerCallback, iInputIsSpatial, iOutputIsSpatial, inputUsage, outputUsage, inputContentType, outputContentType, sampleRateConversionQuality);
    initialized = true;
    }

    @Nullable
    @RequiresPermission(Manifest.permission.RECORD_AUDIO)
    public ByteBuffer getInput() throws Exception {

        if(!initialized){
            throw new Exception("Audio device not initialized");
        }

        if(inputDevice == 0){
            throw new Exception("Stream is not a input");
        }

        ByteBuffer input;
        try {
            input = readInput();
        } catch(Exception e){
            if(initialized) {
                this.terminate();
            }
            return null;
        }

        input.order(ByteOrder.nativeOrder());
        return input;

    }

    public void sendOutput(ByteBuffer samples) throws Exception {

        if(!initialized){
            throw new Exception("Audio device not initialized");
        }

        if(outputDevice == 0){
            throw new Exception("Stream is not a output");
        }

        try {
            writeOutput(samples);
        } catch (Exception e){
            if(initialized){
                this.terminate();
            }
        }

    }

    @Nullable
    public int[] getSampleRates() throws Exception {
        if(!initialized){
            throw new Exception("Audio device not initialized");
        }

        int[] sampleRates;
        try {
            sampleRates = getSampleRate();
        } catch(Exception e){
            if(initialized){
                this.terminate();
            }
            return null;
        }

        return sampleRates;

    }

    @Nullable
    public int[] getChannelCounts() throws Exception {
        if(!initialized){
            throw new Exception("Audio device not initialized");
        }

        int[] channelCounts;

        try {
            channelCounts = getChannelCount();
        } catch(Exception e){
            if(initialized){
                this.terminate();
            }
            return null;
        }

        return channelCounts;

    }

    @Nullable
    public int[] getChannelMasks() throws Exception {
        if(!initialized){
            throw new Exception("Audio device not initialized");
        }

        int[] channelMasks;

        try {
            channelMasks = getChannelMask();
        } catch(Exception e){
            if(initialized){
                this.terminate();
            }
            return null;
        }

        return channelMasks;

    }

    @Nullable
    public int[] getDeviceIds() throws Exception {
        if(!initialized){
            throw new Exception("Audio device not initialized");
        }

        int[] deviceIds;
        try {
            deviceIds = getDeviceId();
        } catch(Exception e){
            if(initialized){
                this.terminate();
            }
            return null;
        }

        return deviceIds;

    }

    @Nullable
    public int[] getFormats() throws Exception {
        if(!initialized){
            throw new Exception("Audio device not initialized");
        }

        int[] format;
        try {
            format = getFormat();
        } catch(Exception e){
            if(initialized){
                this.terminate();
            }
            return null;
        }

        return format;

    }

    @Nullable
    public int[] getContentTypes() throws Exception {
        if(!initialized){
            throw new Exception("Audio device not initialized");
        }

        int[] contentTypes;
        try {
            contentTypes = getContentType();
        } catch(Exception e){
            if(initialized){
                this.terminate();
            }
            return null;
        }

        return contentTypes;

    }

    @Nullable
    public boolean[] getSharingModes() throws Exception {
        if(!initialized){
            throw new Exception("Audio device not initialized");
        }

        boolean[] sharingModes;
        try {
            sharingModes = getSharingMode();
        } catch(Exception e){
            if(initialized){
                this.terminate();
            }
            return null;
        }

        return sharingModes;

    }

    public void setInputFormat(int format) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(inputDevice == 0){
            throw new Exception("Audio device not an input device");
        }

        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.S && format == org.tpkarras.basicoboe.constants.Format.IEC61937) {
            throw new Exception("Attempted to use format IEC61937 with an unsupported API version");
        }

        if(!ObjectMatch((Integer)format, new Integer[]{org.tpkarras.basicoboe.constants.Format.I16, org.tpkarras.basicoboe.constants.Format.Float, org.tpkarras.basicoboe.constants.Format.I24, org.tpkarras.basicoboe.constants.Format.I32, org.tpkarras.basicoboe.constants.Format.IEC61937, org.tpkarras.basicoboe.constants.Format.MP3, org.tpkarras.basicoboe.constants.Format.AAC_LC, org.tpkarras.basicoboe.constants.Format.AAC_HE_V1, org.tpkarras.basicoboe.constants.Format.AAC_HE_V2, org.tpkarras.basicoboe.constants.Format.AAC_ELD, org.tpkarras.basicoboe.constants.Format.AAC_XHE, org.tpkarras.basicoboe.constants.Format.OPUS})){
            throw new Exception("Not a valid audio API");
        }

        inputFormat = format;

    }

    public void setOutputFormat(int format) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(outputDevice == 0){
            throw new Exception("Audio device not an output device");
        }

        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.S && format == Format.IEC61937) {
            throw new Exception("Attempted to use format IEC61937 with an unsupported API version");
        }

        if(!ObjectMatch((Integer)format, new Integer[]{Format.Unspecified, Format.I16, Format.Float, Format.I24, Format.I32, Format.IEC61937, Format.MP3, Format.AAC_LC, Format.AAC_HE_V1, Format.AAC_HE_V2, Format.AAC_ELD, Format.AAC_XHE, Format.OPUS})){
            throw new Exception("Not a valid audio API");
        }

        outputFormat = format;

    }

    public void setInputSampleRate(int sampleRate) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(inputDevice == 0){
            throw new Exception("Audio device not an input device");
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

        InputSampleRate = sampleRate;

    }

    public void setOutputSampleRate(int sampleRate) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(outputDevice == 0){
            throw new Exception("Audio device not an output device");
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

        OutputSampleRate = sampleRate;

    }

    public void setInputChannelCount(int channels) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(inputDevice == 0){
            throw new Exception("Audio device not an input device");
        }

        if(channels < 0){
            throw new Exception("Invalid channel count");
        }

        InputChannelCount = channels;

    }

    public void setOutputChannelCount(int channels) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(outputDevice == 0){
            throw new Exception("Audio device not an output device");
        }

        if(channels < 0){
            throw new Exception("Invalid channel count");
        }

        OutputChannelCount = channels;

    }

    public void setInputChannelMask(@NonNull int[] channelMasks) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(inputDevice == 0){
            throw new Exception("Audio device not an input device");
        }

        if(channelMasks.length == 0){
            throw new Exception("Channel masks cannot be empty.");
        }

        if(channelMasks.length == 1 && channelMasks[0] == org.tpkarras.basicoboe.constants.ChannelMask.Stereo) {

            InputChannelMask = channelMasks[0];

        } else {

            int[] masks = {org.tpkarras.basicoboe.constants.ChannelMask.Unspecified, org.tpkarras.basicoboe.constants.ChannelMask.FrontLeft, org.tpkarras.basicoboe.constants.ChannelMask.FrontRight, org.tpkarras.basicoboe.constants.ChannelMask.FrontCenter, org.tpkarras.basicoboe.constants.ChannelMask.LowFrequency, org.tpkarras.basicoboe.constants.ChannelMask.BackLeft, org.tpkarras.basicoboe.constants.ChannelMask.BackLeft, org.tpkarras.basicoboe.constants.ChannelMask.BackRight, org.tpkarras.basicoboe.constants.ChannelMask.FrontLeftOfCenter, org.tpkarras.basicoboe.constants.ChannelMask.FrontRightOfCenter, org.tpkarras.basicoboe.constants.ChannelMask.BackCenter, org.tpkarras.basicoboe.constants.ChannelMask.SideLeft, org.tpkarras.basicoboe.constants.ChannelMask.SideRight};
            int tempChannelMask = 0;
            ArrayList<Integer> used = new ArrayList<>();

            for (int channelMask : channelMasks) {

                boolean valid = false;

                for (int mask : masks) {

                    if (mask == channelMask) {

                        for(Integer usedC : used) {

                            if(usedC == mask){
                                throw new Exception("Cannot reuse channel mask");
                            }

                        }

                        valid = true;

                        used.add(channelMask);
                        break;
                    }

                }

                if(!valid){
                    throw new Exception("Not a valid channel mask");
                }

                tempChannelMask |= channelMask;

            }

            InputChannelMask = tempChannelMask;

        }

    }

    public void setOutputChannelMask(int[] channelMasks) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(outputDevice == 0){
            throw new Exception("Audio device not an output device");
        }

        if(channelMasks.length == 0){
            throw new Exception("Channel masks cannot be empty.");
        }

        if(channelMasks.length == 1 && channelMasks[0] == org.tpkarras.basicoboe.constants.ChannelMask.Stereo) {

            OutputChannelMask = channelMasks[0];

        } else {

            int[] masks = {org.tpkarras.basicoboe.constants.ChannelMask.Unspecified, org.tpkarras.basicoboe.constants.ChannelMask.FrontLeft, org.tpkarras.basicoboe.constants.ChannelMask.FrontRight, org.tpkarras.basicoboe.constants.ChannelMask.FrontCenter, org.tpkarras.basicoboe.constants.ChannelMask.LowFrequency, org.tpkarras.basicoboe.constants.ChannelMask.BackLeft, org.tpkarras.basicoboe.constants.ChannelMask.BackLeft, org.tpkarras.basicoboe.constants.ChannelMask.BackRight, org.tpkarras.basicoboe.constants.ChannelMask.FrontLeftOfCenter, org.tpkarras.basicoboe.constants.ChannelMask.FrontRightOfCenter, org.tpkarras.basicoboe.constants.ChannelMask.BackCenter, org.tpkarras.basicoboe.constants.ChannelMask.SideLeft, org.tpkarras.basicoboe.constants.ChannelMask.SideRight};
            int tempChannelMask = 0;
            ArrayList<Integer> used = new ArrayList<>();

            for (int channelMask : channelMasks) {

                boolean valid = false;

                for (int mask : masks) {

                    if (mask == channelMask) {

                        for(Integer usedC : used) {

                            if(usedC == mask){
                                throw new Exception("Cannot reuse channel mask");
                            }

                        }

                        valid = true;

                        used.add(channelMask);
                        break;

                    }

                }

                if(!valid){
                    throw new Exception("Not a valid channel mask");
                }

                tempChannelMask |= channelMask;

            }

            OutputChannelMask = tempChannelMask;

        }

    }

    public void setAudioAPI(int audioAPI) throws Exception{

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(!ObjectMatch((Integer)audioAPI, new Integer[]{AudioAPIs.AAudio, AudioAPIs.OpenSL_ES, AudioAPIs.Unspecified})){
            throw new Exception("Not a valid audio API");
        }

        AudioAPI = audioAPI;

    }

    public void setPerformanceMode(int mode) throws Exception{

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(!ObjectMatch((Integer)mode, new Integer[]{PerformanceMode.None, PerformanceMode.LowLatency, PerformanceMode.PowerSaving, PerformanceMode.PowerSavingOffloaded})){
            throw new Exception("Not a valid performance mode");
        }

        performanceMode = mode;

    }

    public void setInputSharingMode(boolean mode) throws Exception{

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(inputDevice == 0){
            throw new Exception("Audio device not an input device");
        }

        inputSharingMode = mode;

    }

    public void setOutputSharingMode(boolean mode) throws Exception{

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(outputDevice == 0){
            throw new Exception("Audio device not an output device");
        }

        outputSharingMode = mode;

    }

    public void setFramesPerCallback(int frames) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(frames < 0){
            throw new Exception("Frames cannot be less than 0");
        }

        framesPerCallback = frames;

    }

    public void setInputSpatialization(boolean isSpatial) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        InputIsSpatial = isSpatial;

    }

    public void setOutputSpatialization(boolean isSpatial) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        OutputIsSpatial = isSpatial;

    }

    public void setInputUsage(int usage) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(!ObjectMatch((Integer)usage, new Integer[]{0, Usage.Alarm, Usage.AssistanceAccessibility, Usage.VoiceCommunicationSignalling, Usage.Notification, Usage.AssistanceNavigationGuidance, Usage.Assistant, Usage.AssistanceSonification, Usage.Game, Usage.Media, Usage.NotificationRingtone, Usage.VoiceCommunication, Usage.NotificationEvent})){
            throw new Exception("Not a valid audio API");
        }

        InputUsage = usage;

    }

    public void setOutputUsage(int usage) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(!ObjectMatch((Integer)usage, new Integer[]{0, Usage.Alarm, Usage.AssistanceAccessibility, Usage.VoiceCommunicationSignalling, Usage.Notification, Usage.AssistanceNavigationGuidance, Usage.Assistant, Usage.AssistanceSonification, Usage.Game, Usage.Media, Usage.NotificationRingtone, Usage.VoiceCommunication, Usage.NotificationEvent})){
            throw new Exception("Not a valid audio API");
        }

        OutputUsage = usage;

    }

    public void setInputContentType(int contentType) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(!ObjectMatch((Integer)contentType, new Integer[]{0, ContentType.Speech, ContentType.Music, ContentType.Movie, ContentType.Sonification})){
            throw new Exception("Not a valid content type");
        }

        InputContentType = contentType;

    }

    public void setOutputContentType(int contentType) throws Exception {

        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(!ObjectMatch((Integer)contentType, new Integer[]{0, ContentType.Speech, ContentType.Music, ContentType.Movie, ContentType.Sonification})){
            throw new Exception("Not a valid content type");
        }

        OutputContentType = contentType;

    }

    public void setSampleRateConversionQuality(int mode) throws Exception{
        if(initialized){
            throw new Exception("Audio device already initialized");
        }

        if(!ObjectMatch((Integer)mode, new Integer[]{SampleRateConversionQuality.None, SampleRateConversionQuality.Fastest, SampleRateConversionQuality.Low, SampleRateConversionQuality.Medium, SampleRateConversionQuality.High, SampleRateConversionQuality.Best})){
            throw new Exception("Not a valid sample rate conversion quality mode");
        }

        iSampleRateConversionQuality = mode;
    }

}
