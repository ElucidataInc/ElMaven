#read in table of peaks in summary matrix format 
args <- commandArgs(trailingOnly = TRUE); groupsTableFile = args[1];

cat("Importing ", groupsTableFile, "\n");
GROUPS = read.csv(groupsTableFile,row.names=NULL);

cat("Read in", nrow(GROUPS), " peak groups\n");

SAMPLECOLMS = 14:ncol(GROUPS)
    SAMPLENAMES = names(GROUPS)[SAMPLECOLMS];
    SAMPLECOLORS = rainbow(length(SAMPLECOLMS));

    pdf("barplots.pdf");
    par(mfrow=c(2,2));
    for( i in 1:nrow(GROUPS)) { 
        y=as.numeric(GROUPS[i,SAMPLECOLMS]);
        y[is.na(y)] = 0;
        plotTitle = paste(GROUPS[i,8]);   #column 8 = compound name

        cat("Plotting ", plotTitle, "\n");
        barplot(y,main=plotTitle,names.arg=SAMPLENAMES,las=3,col=SAMPLECOLORS);
    }
dev.off();

cat("DONE :)");


