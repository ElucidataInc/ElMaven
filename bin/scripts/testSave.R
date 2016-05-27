args <- commandArgs(trailingOnly = TRUE) #version 3
groupReport = args[1];

cat("Importing ", groupReport, "\n");
PEAKS = read.csv(groupReport,row.names=NULL);
print(dim(PEAKS));

SAMPLES = 14:17

#pdf("/tmp/data.pdf");
par(mfrow=c(5,5), mar=c(0,0,0,5));
for( i in 1:nrow(PEAKS)) { 
    y=as.numeric(PEAKS[i,SAMPLES]);
    y[is.na(y)] = 0;
    print(mean(y));
    #barplot(y);
}
#dev.off();

cat("DONE..");
