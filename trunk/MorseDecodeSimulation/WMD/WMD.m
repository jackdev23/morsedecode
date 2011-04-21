function varargout = WMD(varargin)
% WMD MATLAB code for WMD.fig
%      WMD, by itself, creates a new WMD or raises the existing
%      singleton*.
%
%      H = WMD returns the handle to a new WMD or the handle to
%      the existing singleton*.
%
%      WMD('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in WMD.M with the given input arguments.
%
%      WMD('Property','Value',...) creates a new WMD or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before WMD_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to WMD_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help WMD

% Last Modified by GUIDE v2.5 21-Apr-2011 11:21:04

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @WMD_OpeningFcn, ...
                   'gui_OutputFcn',  @WMD_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before WMD is made visible.
function WMD_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to WMD (see VARARGIN)
global pathName fileName GuiDir fsig magnitude_value hd SpaceWidth
pathName = cd;
GuiDir   = cd;
fileName = '';
fsig = 600;
magnitude_value = -24;
% Choose default command line output for WMD
handles.output = hObject;
load hd;
SpaceWidth = 2.5;
% Update handles structure
guidata(hObject, handles);

% UIWAIT makes WMD wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = WMD_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



function edit1_Callback(hObject, eventdata, handles)
% hObject    handle to edit1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% Hints: get(hObject,'String') returns contents of edit1 as text
%        str2double(get(hObject,'String')) returns contents of edit1 as a double
% global fsig
% fsig = str2double(get(hObject,'String'));


% --- Executes during object creation, after setting all properties.
function edit1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
  
  global pathName fileName GuiDir
  global x fsample fileName 
  % When 'browse...' button is clicked in preview panel
        
        cd (pathName); % Change to current or last directory looked at
        currentPathName=pathName;
        currentFileName=fileName;
        [fileName,pathName]= uigetfile({'*.wav';'*.*'},'Text File'); % Get file name

        if isequal(fileName,0) || isequal(pathName,0)
            % Do nothing if fails or exists without chooseing any file
            pathName=currentPathName; % Put back
            fileName=currentFileName; % Put back
        else
            % Do something with the file chosen
            currentPathName=pathName;
            currentFileName=fileName;
            cd(pathName);
            % Get File data and column names
            try 
                [x,fsample,NBITS]=wavread(fileName);
                set(handles.edit1,'String',[currentPathName currentFileName]);
                 if(size(x,2)==2)  %% if in stereo strip to mono
                    x=x(:,1);
                 end
                 if(fsample ~= 44100)
                     set(handles.edit1,'String','ERROR - Wave file not at Frequency 44100.');
                 end
            catch
                set(handles.edit1,'String','ERROR - File could not open.');
            end           
        end        
        cd(GuiDir);


% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global x fsample fsig fileName hd SpaceWidth
global GuiDir magnitude_value xn
cd(GuiDir);
%% DECIMATE
set(handles.edit3,'String','Decimate Signal');
decimate_rate = 10;
if(fsample == 44100)
    try
        xn=decimate(x,decimate_rate,'fir');
    catch
        set(handles.edit3,'String','ERROR: Failed to Decimate Signal');
        return;
    end
    fsample = fsample/decimate_rate;
    clear x;
end
L = length(xn);

set(handles.edit3,'String','Filtering');
 %% Frequency Shift Transform - simplified.
% does the same as above but broken out into multiple parts to make it
% easier to understand for uProcessor implementation.
mixerI=sin(2*pi*fsig/fsample .* (0:L-1))';
mixerR=cos(2*pi*fsig/fsample .* (0:L-1))';
sigR = xn.*mixerR;
sigI = xn.*mixerI;
clear mixerI
clear mixerR
% clear xn

set(handles.edit3,'String','Loaded Filter');
%% Low Pass Filter - Simplified
% filter both the complex and real number then calc magnitude of results
fxnI = filter(hd,sigI);  % filter imaginary component
fxnR = filter(hd,sigR);  % filter real component
 clear sigR
 clear sigI
fxn = sqrt(fxnR.^2 + fxnI.^2); % calculate magnitude of Img & Real data 
% fxn is strictly real now.

set(handles.edit3,'String','Window Filter');
%% Window Averaging Filter - Additional Filtering
Nw = 100;
mavgwindow = ones(1,Nw)/Nw; % essentially average over Nw = 100 size window
% fxn_before_avg=fxn;
fxn = filter(mavgwindow,1,abs(fxn)); % averging window cleans up results

set(handles.edit3,'String','Plotting Signal');
figure(1);
plot(fxn); 
title(fileName); xlabel('Signal'); ylabel('Samples');
set(handles.edit3,'String','Complete');

Ts = 1/fsample;
[stringArray] = mag_detect_wave(fxn, magnitude_value, 1, Ts, SpaceWidth);
set(handles.edit4,'String',stringArray);
MorseString = morse_loopup(stringArray);
set(handles.edit5,'String',MorseString);



function edit2_Callback(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit2 as text
%        str2double(get(hObject,'String')) returns contents of edit2 as a double


% --- Executes during object creation, after setting all properties.
function edit2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on key press with focus on edit2 and none of its controls.
function edit2_KeyPressFcn(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  structure with the following fields (see UICONTROL)
%	Key: name of the key that was pressed, in lower case
%	Character: character interpretation of the key(s) that was pressed
%	Modifier: name(s) of the modifier key(s) (i.e., control, shift) pressed
% handles    structure with handles and user data (see GUIDATA)
global fsig
fsig = str2double(get(hObject,'String'));


% --- Executes on button press in pushbutton3.
function pushbutton3_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global fsig
fsig = str2double(get(handles.edit2,'String'));

% --- If Enable == 'on', executes on mouse press in 5 pixel border.
% --- Otherwise, executes on mouse press in 5 pixel border or over pushbutton3.
function pushbutton3_ButtonDownFcn(hObject, eventdata, handles)
% hObject    handle to pushbutton3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global fsig
fsig = str2double(get(handles.edit2,'String'));



function edit3_Callback(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit3 as text
%        str2double(get(hObject,'String')) returns contents of edit3 as a double


% --- Executes during object creation, after setting all properties.
function edit3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit4_Callback(hObject, eventdata, handles)
% hObject    handle to edit4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit4 as text
%        str2double(get(hObject,'String')) returns contents of edit4 as a double


% --- Executes during object creation, after setting all properties.
function edit4_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit5_Callback(hObject, eventdata, handles)
% hObject    handle to edit5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes during object creation, after setting all properties.
function edit5_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit6_Callback(hObject, eventdata, handles)
% hObject    handle to edit6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global magnitude_value
magnitude_value = str2double(get(hObject,'String'));


% --- Executes during object creation, after setting all properties.
function edit6_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton4.
function pushbutton4_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global magnitude_value
magnitude_value = str2double(get(handles.edit6,'String'));




% --- Executes during object creation, after setting all properties.
function edit7_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit7_Callback(hObject, eventdata, handles)
% hObject    handle to edit7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit7 as text
%        str2double(get(hObject,'String')) returns contents of edit7 as a double


% --- Executes on button press in pushbutton5.
function pushbutton5_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global SpaceWidth
SpaceWidth = str2double(get(handles.edit7,'String'));
