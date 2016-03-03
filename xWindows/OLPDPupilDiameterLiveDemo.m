function [data, params] = OLPDPupilDiameterLiveDemo
% [data, params] = OLPDPupilDiameterSubjectWindows
%
% This function works in lockstep with the program
% BasicPupilDiameter that runs on the Mac host.
% It uses UDP to talk with the Mac, and controls
% the VSG eye tracker to do the Mac's bidding.
%
% xx/xx/12  dhb, sj         Written.
% 07/25/13  ms              Commented.

%% Initializing Cambridge Researsh System and Other Neccessary Variables
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

% Creates a delay to allow the camera screen to finish loading loading
vetCreateCameraScreen;

vetStartTracking;
%pause(5)
%dataTest = vetGetBufferedEyePositions;
vetStopTracking;