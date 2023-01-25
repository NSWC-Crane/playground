% Reads Heatmap table for blur count
% Plots focus points across range
% Creates .mat file with heatmap matrix, focus, range, and zoom

clc
clear
zoom = 5000;
rangeV= 600:50:1000;
% Read in the heatmap table
filen = "C:\Data\JSSAP\Results_new_CB\tbHeatmap110N_" + num2str(zoom) + ".csv";
TbHeatm = readtable(filen);


% plots
p=figure(100);
legendTxt = [];
f = 48080:5:48090; %5:48155;%;47990;
%plotColors = jet(length(f));
plotColors = ["#0072BD", "#D95319", "#EDB120", "#7E2F8E", "#77AC30","#4DBEEE","#A2142F","#22AC30"];
for i = 1:length(f)
    tbPlot = TbHeatm(TbHeatm.Focus5 == f(i),:);
    plot(tbPlot.Range,tbPlot.BlurPix,'-*','Color', plotColors(i), 'LineWidth', 2);
    legendTxt = [legendTxt;num2str(f(i))];
    hold on
end
xticklabels(rangeV)
xticks(rangeV)
yticks(3:0.5:10.5)
xlabel("Range")
ylabel("Number of Blurred Pixels")
title("Zoom " + num2str(zoom)+ " Focus " + num2str(f(1)) + "-" + num2str(f(end)))
legend(legendTxt,Location="eastoutside")
xlim([rangeV(1),rangeV(end)])
ylim([3,10.5])
grid on
hold off
% fileOut = "C:\Data\JSSAP\Results_new_CB\p11_z" + num2str(zoom) + "_f" + num2str(f(1)) + ".png";
% exportgraphics(p,fileOut,'Resolution',300)



%% Create output matrix from TbHeatm
% Define focus range
focusVals = unique(TbHeatm.Focus5);
startFocus = min(focusVals);
endFocus = max(focusVals);
focusI = startFocus:5:endFocus;
fileMat = "C:\Data\JSSAP\Results_new_CB\sample_blur_radius_data_z" + num2str(zoom) + ".mat";
for rInd = 1:length(rangeV)
    for fInd = 1:length(focusI)
        indB = find(TbHeatm.Range == rangeV(rInd) & TbHeatm.Focus5 == focusI(fInd));
        newMat(fInd, rInd) = TbHeatm.BlurPix(indB);
    end
end
range = rangeV;
coc_map = newMat;
focus = focusI;

save(fileMat, 'coc_map', 'focus','range','zoom')