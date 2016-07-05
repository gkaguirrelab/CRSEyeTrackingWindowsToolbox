% Housekeeping.
clear; close all; clc

recordingTimeMinutes = 30;
recordingTimeSeconds = 5;
nChunks = 6;

% Initialize Cambridge Research System and Other Neccessary Variables
% Global CRS gives us access to a cell structure of the Video Eye Tracker's
% variables.  Load constants creates this cell structure
global CRS;
if isempty(CRS)
    crsLoadConstants;
end

% vetClearDataBuffer clears values that may have been previously recorded
vetClearDataBuffer;

% vetLoadCalibrationFile loads a calibration file that was created using the
% provided CRS application called Video Eye Trace.  This calibration file
% correlates a subject's pupil position with a focal point in visual space.
% The .scf file is needed in order for the Eye tracker to intialize and
% function properly.
calFilePath = 'C:\Users\melanopsin\Documents\MATLAB\Toolboxes\PupillometryToolbox\xWindows\subjectcalibration_current.scf';
vetLoadCalibrationFile(calFilePath);

% The way CRS setup the Eye Tracker, we must set a stimulus device, although
% in reality, our stimulus device is the OneLight machine. For the sake of
% initialization, we must tell the Video Eye Tracker that the stimulus will
% be presented on a screen connected through a VGA port.
vetSetStimulusDevice(CRS.deVGA);

% vetSelectVideoSource prepares the framegrabber (PICOLO card) to receive
% data from a connected video eye tracker.  Our model of the eye tracker is
% labeled as the .vsCamera (a CRS convention/nomenclature)
if vetSelectVideoSource(CRS.vsCamera) < 0
    error('*** Video source not selected.');
end

protocolNameStr = 'MelanopsinMR_AbsolutePupilSize';
obsID = GetWithDefault('> Enter observer ID', 'HERO_xxx1');

dropboxPath = 'C:\Users\melanopsin\Dropbox (Aguirre-Brainard Lab)\MELA_data';
savePath = fullfile(dropboxPath, protocolNameStr, obsID, datestr(now, 'mmddyy'), 'MatFiles');
if ~exist(savePath);
    mkdir(savePath);
end



vetCreateCameraScreen;

% Clear the data buffer;
fprintf('\n>> PRESS ENTER TO START RECORDING.');
pause;
tic;
fprintf('\n>> Starting recording now.')
vetClearDataBuffer;
vetStartTracking;
for i = 1:nChunks
    % Set up the file name of the output file
    saveFile = fullfile(savePath, [obsID '-' datestr(now, 'mmddyy') '-' num2str(i) '.mat']);
    pause(recordingTimeSeconds);
    data = vetGetBufferedEyePositions;
    toc;
    fprintf('\n>> Recording done');
    save(saveFile, 'data');
end
fprintf('\n>> EXPERIMENT DONE');
vetStopTracking;
beep