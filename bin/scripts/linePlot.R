#read in table of peaks in summary matrix format 
args <- commandArgs(trailingOnly = TRUE); groupsTableFile = args[1];

cat("Importing ", groupsTableFile, "\n");
GROUPS = read.csv(groupsTableFile,row.names=NULL);

cat("Read in", nrow(GROUPS), " peak groups\n");

SAMPLECOLMS = 14:ncol(GROUPS)
SAMPLENAMES = names(GROUPS)[SAMPLECOLMS];
SAMPLECOLORS = rainbow(length(SAMPLECOLMS));

group1Means = rowMeans(GROUPS[,14:15])+1;
group2Means = rowMeans(GROUPS[,16:17])+1;


windows(); #open plot window

##alternative option.. open interactive plot window
#png("c:\\temp\\lineplot.png",width=800,height=800); 

plot(group1Means,group2Means,cex=0.9,pch=19,log="xy", xlab="Group1 Mean", ylab="Group2 Mean");
text(group1Means,group2Means,GROUPS[,9],pos=4); 
locator(1e6);

#automatically open PNG file
#browseURL("c:\\temp\\lineplot.png")

cat("DONE :)");


