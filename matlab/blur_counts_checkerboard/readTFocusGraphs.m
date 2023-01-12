
clc
clear
zoom = 5000;
rangeV= 600:50:1000;
% Read in the heatmap
filen = "C:\Data\JSSAP\Results_new_CB\tbHeatmap110_" + num2str(zoom) + ".csv";
TbHeatm = readtable(filen);


% plots
p=figure(100);
legendTxt = [];
f = 48150:5:48155;%;47990;
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
xlabel("Range")
ylabel("Number of Blurred Pixels")
title("Zoom " + num2str(zoom)+ " Focus " + num2str(f(1)) + "-" + num2str(f(end)))
legend(legendTxt,Location="eastoutside")
xlim([rangeV(1),rangeV(end)])
ylim([4,10])
grid on
hold off
fileOut = "C:\Data\JSSAP\Results_new_CB\p11_z" + num2str(zoom) + "_f" + num2str(f(1)) + ".png";
exportgraphics(p,fileOut,'Resolution',300)

% Notes:
% z2000: f46150: 46345
% z2500: f46530:46715
% z3000: f46940: 47145
% z3500: f47350: 47545
% z4000: F47630: 47790
% z5000: f47940: 48190

%% Create output matrix from TbHeatm
% Define focus range
focusVals = unique(TbHeatm.Focus5);
startFocus = min(focusVals);
endFocus = max(focusVals);
focusI = startFocus:5:endFocus;
fileMat = "C:\Data\JSSAP\results_CB_focusRange\sample_blur_radius_data5_z" + num2str(zoom) + ".mat";
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