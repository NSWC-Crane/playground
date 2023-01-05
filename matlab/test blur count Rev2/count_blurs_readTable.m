clc
clear

% Save Tb table
filename = "C:\Data\JSSAP\20220908_101308\Rework\R100TbAll_1slice.csv";
%writetable(Tb, "C:\Data\JSSAP\20220908_101308\TbAll.csv");

% Read Tb
Tb = readtable(filename);

%% Round focus to nearest 10s column
% Add column in table for rounded value
%Tb.focusRnd10 = round(Tb.Focus,-1);

%% Create heatmap
rangeV = 500:10:750;

% Group data by focus intervals
startFocus = 46500;
endFocus = 47340;
intvF = 10;

focusI = startFocus:intvF:endFocus;
rowsH = (length(rangeV)-1) * length(focusI);
vartypesH = {'uint16', 'uint16', 'double'};
TbHeatm = table('Size', [rowsH,3], 'VariableTypes', vartypesH);
TbHeatm.Properties.VariableNames = ["Range", "FocusItv", "BlurPix"];
indH = 1;
for rng = rangeV
    for fc = 1:length(focusI)-1 % One less interval than number of focus points
        %indF = find(Tb.Range == rng & Tb.focusRnd10 == focusI(fc)); % & Tb.focusRnd10 < focusI(fc+1) );
        indF = find(Tb.Range == rng & Tb.Focus >= focusI(fc) & Tb.Focus < focusI(fc+1) );
        TbHeatm(indH,:) = {rng, focusI(fc), mean(Tb.BlurMean(indF))};
        TbHeatm(indH,:) = {rng, focusI(fc), mean(Tb.BlurMean(indF))};
        indH = indH + 1;
    end
end
%writetable(TbHeatm, "C:\Data\JSSAP\20220908_101308\Rework\R100TbHeatmap10s_1slice.csv");

fig = figure(5);
h = heatmap(TbHeatm, 'Range', 'FocusItv', 'ColorVariable', 'BlurPix','Colormap', parula);
xlabel("Range")
ylabel("Focus Interval")
title("Mean Blurred Pixels per Range and Focus Interval")
set(gcf,'position',([100,100,1100,1500]),'color','w')
fileOut = "C:\Data\JSSAP\20220908_101308\Rework\R100heatmap10s_1slice.png";
%exportgraphics(h,fileOut,'Resolution',300)
fileFig = "C:\Data\JSSAP\20220908_101308\Rework\R100heatmap10s_1slice.fig";
%savefig(fig, fileFig)

%% Create output matrix
fileMat = "C:\Data\JSSAP\20220908_101308\Rework\sample_blur_radius_data2.mat";
for rInd = 1:length(rangeV)
    for fInd = 1:length(focusI)-1
        indB = find(TbHeatm.Range == rangeV(rInd) & TbHeatm.FocusItv == focusI(fInd));
        newMat(fInd, rInd) = TbHeatm.BlurPix(indB);
    end
end
range = rangeV;
coc_map = newMat;
zoom = [2000];
focus = focusI(1:end-1);

%save(fileMat, 'coc_map', 'focus','range','zoom')
