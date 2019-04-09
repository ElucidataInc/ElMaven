#include "mzUtils.h"
#include "SavGolSmoother.h"
#include "csvparser.h"
#include "masscutofftype.h"

/**
 * random collection of useful functions 
 */
namespace mzUtils {

    std::string makeLowerCase(string &s) {
        for (unsigned int i=0; i != s.length(); ++i ) {
            s[i] = std::tolower(s[i]);
        }
        return s;
    }

    void split(const string& s, char c, vector<string>& v) {
        string::size_type i = 0;
        string::size_type j = s.find(c);

        while (j != string::npos) {
            v.push_back(s.substr(i, j-i));
            i = ++j;
            j = s.find(c, j);

            if (j == string::npos)
                v.push_back(s.substr(i, s.length( )));
        }
        if ( v.size() == 0) v.push_back(s);
    }

    void splitNew(const string& s, const string& c, vector<string>& v){

        const char *whole_row = s.c_str();
        const char *del = c.c_str();

        CsvParser *csvparser = CsvParser_new_from_string(whole_row, del, 0);

        CsvRow *row;

        row = CsvParser_getRow(csvparser);
        const char **rowFields = CsvParser_getFields(row);
        for (int i = 0 ; i < CsvParser_getNumFields(row) ; i++) {
            v.push_back(rowFields[i]);
        }
    }

    void removeSpecialcharFromStartEnd(vector<string>& fields) {
        for(unsigned int i=0; i < fields.size(); i++ ) {
            int n = fields[i].length();
            if (n > 2 && fields[i][0] == '"' && fields[i][n - 1] == '"') {
                fields[i] = fields[i].substr(1, n - 2);
            }
            if (n > 2 && fields[i][0] == '\'' && fields[i][n - 1] == '\'') {
                fields[i] = fields[i].substr(1, n - 2);
            }
        }
    }

    char *mystrcasestr(const char *s1, const char *s2) {
        register const char *s = s1;
        register const char *p = s2;
        do {
            if (!*p) {
                return (char *) s1;;
            }
            if ((*p == *s)
                    || (tolower(*((unsigned char *)p)) == tolower(*((unsigned char *)s)))
               ) {
                ++p;
                ++s;
            } else {
                p = s2;
                if (!*s) {
                    return NULL;
                }
                s = ++s1;
            }
        } while (1);
    }

    string substituteInQuotedString(const string& s, const string& chars, const string& substitutions ) {
        string result;
        for (string::size_type pos = 0; pos < s.size(); ++pos) {
            char c = s[pos];
            string::size_type subst_pos = chars.find_first_of(c);
            if (subst_pos != string::npos) c = substitutions[subst_pos];
            result += c;
        }
        return result;
    }

    void smoothAverage(float *y, float* s, int smoothWindowLen, int ly) {
        if (smoothWindowLen == 0 ) return;
        float* x = new float[smoothWindowLen];
        for(int i=0; i< smoothWindowLen; i++ ) x[i] = 1.0/smoothWindowLen;
        conv(smoothWindowLen,-smoothWindowLen/2,x,ly, 0,y,ly,0,s);
        delete[] x;
    }

    void conv (int lx, int ifx, float *x, int ly, int ify, float *y, int lz, int ifz, float *z) /*****************************************************************************
                                                                                                  Compute z = x convolved with y; i.e.,

                                                                                                  ifx+lx-1
                                                                                                  z[i] =   sum    x[j]*y[i-j]  ;  i = ifz,...,ifz+lz-1
                                                                                                  j=ifx
                                                                                                 ******************************************************************************
                                                                                                 Input:
                                                                                                 lx		length of x array
                                                                                                 ifx		sample index of first x
                                                                                                 x		array[lx] to be convolved with y
                                                                                                 ly		length of y array
                                                                                                 ify		sample index of first y
                                                                                                 y		array[ly] with which x is to be convolved
                                                                                                 lz		length of z array
                                                                                                 ifz		sample index of first z

                                                                                                 Output:
                                                                                                 z		array[lz] containing x convolved with y
                                                                                                 ******************************************************************************
                                                                                                 Notes:
                                                                                                 The x samples are contained in x[0], x[1], ..., x[lx-1]; likewise for
                                                                                                 the y and z samples.  The sample indices of the first x, y, and z values
                                                                                                 determine the location of the origin for each array.  For example, if
                                                                                                 z is to be a weighted average of the nearest 5 samples of y, one might
                                                                                                 use
                                                                                                 ...
                                                                                                 x[0] = x[1] = x[2] = x[3] = x[4] = 1.0/5.0;
                                                                                                 conv(5,-2,x,lx,0,y,ly,0,z);
                                                                                                 ...
                                                                                                 In this example, the filter x is symmetric, with index of first sample = -2.

                                                                                                 This function is optimized for architectures that can simultaneously perform
                                                                                                 a multiply, add, and one load from memory; e.g., the IBM RISC System/6000.
                                                                                                 Because, for each value of i, it accumulates the convolution sum z[i] in a
                                                                                                 scalar, this function is not likely to be optimal for vector architectures.
                                                                                                 ******************************************************************************
                                                                                                 Author:  Dave Hale, Colorado School of Mines, 11/23/91
                                                                                                 *****************************************************************************/
    {
        int ilx=ifx+lx-1,ily=ify+ly-1,ilz=ifz+lz-1,i,j,jlow,jhigh;
        float sum;

        x -= ifx;  y -= ify;  z -= ifz;
        for (i=ifz; i<=ilz; ++i) {
            jlow = i-ily;  if (jlow<ifx) jlow = ifx;
            jhigh = i-ify;  if (jhigh>ilx) jhigh = ilx;
            for (j=jlow,sum=0.0; j<=jhigh; ++j)
                sum += x[j]*y[i-j];
            z[i] = sum;
        }
    }

    void gaussian1d_smoothing (int ns, int nsr, float *data)
    {
        //Subroutine to apply a one-dimensional gaussian smoothing

        /******************************************************************************
Input:
ns		number of samples in the input data
nsr		width (in samples) of the gaussian for which
amplitude > 0.5*max amplitude
data		1-D array[ns] of data to smooth

Output:
data		1-D array[ns] of smoothed data
         ******************************************************************************/
        int is;				/* loop counter */
        float sum=0.0;
        float fcut;
        float r;
        float fcutr=1.0/nsr;
        int n=0;
        int mean=0;
        float fcutl=0;
        float s[1000];		/* smoothing filter array */
        float *temp;			/* temporary array */

        /* save input fcut */
        fcut=fcutr;

        /* don't smooth if nsr equal to zero */
        if (nsr==0 || ns<=1) return;

        /* if halfwidth more than 100 samples, truncate */
        if (nsr>100) fcut=1.0/100;

        /* allocate space */
        temp=new float[ns];


        /* initialize smoothing function if not the same as the last one used */
        if (fcut != fcutl) {
            fcutl=fcut;

            /* set span of 3, at width of 1.5*exp(-PI*1.5**2)=1/1174 */
            n=(int) (3.0/fcut+0.5);
            n=2*n/2+1;		/* make it odd for symmetry */

            /* mean is the index of the zero in the smoothing wavelet */
            mean=n/2;

            /* s(n) is the smoothing gaussian */
            for (is=1; is<=n; is++) {
                r=is-mean-1;
                r= -r*r*fcut*fcut*3.141;
                s[is-1]=exp(r);
            }

            /* normalize to unit area, will preserve DC frequency at full
               amplitude. Frequency at fcut will be half amplitude */
            for (is=0; is<n; is++) sum +=s[is];
            for (is=0; is<n; is++) s[is] /=sum;
            //cerr << "new gaussian " << nsr << endl;
        }

        /* convolve by gaussian into buffer */
        if (1.01/fcutr>(float)ns) {

            /* replace drastic smoothing by averaging */
            sum=0.0;
            for (is=0; is<ns; is++) sum +=data[is];
            sum /=ns;
            for (is=0; is<ns; is++) data[is]=sum;

        } else {

            /* convolve with gaussian */
            conv (n, -mean, s, ns, -mean, data, ns, -mean, temp);

            /* copy filtered data back to output array */
            for (is=0; is<ns; is++) data[is]=temp[is];
        }

        /* free allocated space */
        delete[] temp;
    }

    float median(vector <float> y) {
        if (y.empty() ) return(0.0);
        if (y.size() == 1 ) return(y[0]);
        if (y.size() == 2 ) { return(y[0]+y[1])/2; }

        unsigned int n = y.size();
        std::sort(y.begin(), y.end());

        unsigned int i = n / 2;
        float median = 0.0;

        if (n == i * 2)
            median = (y[i-1] + y[i]) / 2.;
        else
            median = y[i];

        return (median);
    }

    float median(float* y, int n) {
        if (n == 0) return(0.0);
        if (n == 1) return(y[0]);
        if (n == 2) return((y[0]+y[1])/2);

        float* tmpy = new float[n];
        memcpy(tmpy,y,n*sizeof(float));
        sort(tmpy,tmpy+n);

        int i = n / 2;
        float median = 0.0;

        if (n == i * 2)
            median = (tmpy[i-1] + tmpy[i]) / 2.;
        else
            median = tmpy[i];

        delete[] tmpy;
        //for(int i=0; i < n; i++ ) { cerr << tmpy[i] << " "; }
        //cerr << "median=" << median << endl;
        return (median);
    }

    /*
     * The following code is public domain.
     * Algorithm by Torben Mogensen, implementation by N. Devillard.
     * This code in public domain.
     *  In place median, no need to allocate memory
     */
    float torben_median(const vector<float> &m) {
        int         i, less, greater, equal;
        float min, max, guess, maxltguess, mingtguess;
        int n = m.size();
        if (n == 0 ) return 0;
        if (n == 1 ) return m[0];
        min = max = m[0] ;

        for (i=1 ; i<n ; i++) {
            if (m[i]<min) min=m[i];
            if (m[i]>max) max=m[i];
        }
        while (1) {
            guess = (min+max)/2;
            less = 0; greater = 0; equal = 0;
            maxltguess = min ;
            mingtguess = max ;
            for (i=0; i<n; i++) {
                if (m[i]<guess) {
                    less++;
                    if (m[i]>maxltguess) maxltguess = m[i] ;
                } else if (m[i]>guess) {
                    greater++;
                    if (m[i]<mingtguess) mingtguess = m[i] ;
                } else equal++;
            }
            if (less <= (n+1)/2 && greater <= (n+1)/2) break ;
            else if (less>greater) max = maxltguess ;
            else min = mingtguess;
        }
        if (less >= (n+1)/2) return maxltguess;
        else if (less+equal >= (n+1)/2) return guess;
        else return mingtguess;
    }

    /*
     * Algorithm from N. Wirth’s book, implementation by N. Devillard.
     * This code in public domain.
     */
#define ELEM_SWAP(a,b) { register float t=(a);(a)=(b);(b)=t; }
    /*---------------------------------------------------------------------------
Function :   kth_smallest()
:   array of elements, # of elements in the array, rank k
In
:   one element
Out
Job      :   find the kth smallest element in the array
Notice   :   use the median() macro defined below to get the median.
Reference:
Author:  Wirth, Niklaus
Title:  Algorithms + data structures = programs
Publisher:  Englewood Cliffs: Prentice-Hall, 1976
Physical description: 366 p.
Series:  Prentice-Hall Series in Automatic Computation
---------------------------------------------------------------------------*/
    float kth_smallest(float a[], int n, int k)
    {
        register int i,j,l,m ;
        register float x ;
        l=0 ; m=n-1 ;
        while (l<m) {
            x=a[k] ;
            i=l ;
            j=m ;
            do {
                while (a[i]<x) i++ ;
                while (x<a[j]) j-- ;
                if (i<=j) {
                    ELEM_SWAP(a[i],a[j]) ;
                    i++ ; j-- ;
                }
            } while (i<=j) ;
            if (j<k) l=i ;
            if (k<i) m=j ;
        }
        return a[k] ;
    }
#define wirth_median(a,n) kth_smallest(a,n,(((n)&1)?((n)/2):(((n)/2)-1)))
    /* string2integer */
    int string2integer(const std::string& s){
        std::istringstream i(s);
        int x = 0;
        i >> x;
        return x;
    }

    /* string2float */
    float string2float(const std::string& s){
        std::istringstream i(s);
        float x = 0;
        i >> x;
        return x;
    }

    /* string2integer */
    string integer2string(int x){
        std::stringstream i;
        string s;
        i << x;
        i >> s;
        return s;
    }

    string float2string(float f, int p) {
        std::stringstream ss;
        ss << setprecision(p) << f; string str; ss >> str;
        return(str);
    }

    float massCutoffDist(const float mz1, const float mz2,MassCutoff *massCutoff) {
        if(massCutoff->getMassCutoffType()=="ppm"){
            return ( abs((mz2-mz1)/(mz1/1e6)) );
        }
        else if(massCutoff->getMassCutoffType()=="mDa"){
            return abs((mz2-mz1)*1e3) ;
        }
        else{
            assert(false);
            return 0;
        }
    }

    double massCutoffDist(const double mz1, const double mz2,MassCutoff *massCutoff) {

        if(massCutoff->getMassCutoffType()=="ppm"){
            return ppmDist(mz1, mz2);
        }
        else if(massCutoff->getMassCutoffType()=="mDa"){
            return abs((mz2-mz1)*1e3) ;
        }
        else{
            assert(false);
            return 0;
        }
    }

    float ppmDist(const float mz1, const float mz2) {
        return (abs((mz2-mz1)/(mz1/1e6)));
    }

    double ppmDist(const double mz1, const double mz2) {
        return (abs((mz2-mz1)/(mz1/1e6)));
    }

    float ppmround(const float mz1, const float resolution) {
        //resolution parameter =10  -> one digit after decimal point,
        //                      100 -> two digits after decimal point
        //                      etc..

        return( round(mz1*resolution)/resolution);
    }

    bool withinXMassCutoff( float mz1, float mz2, MassCutoff *massCutoff ) {
        if ( mz2 > (mz1 - massCutoff->massCutoffValue(mz1)) && mz2 < (mz1 + massCutoff->massCutoffValue(mz1)) ) return(true);
        else return(false);
    }

    vector<float> quantileDistribution( vector<float> y ) {
        int ysize = y.size();
        std::sort(y.begin(), y.end());	//sort y
        vector<float> quantiles(101,0);
        for (int i=0; i < 101; i++ ) {
            int pos = (float) i/100 * ysize;
            if (pos < ysize) quantiles[i] = y[pos];
        }
        return(quantiles);
    }


    float ttest(StatisticsVector<float>& groupA, StatisticsVector<float>& groupB ) {
        int n1 = groupA.size();
        int n2 = groupB.size();
        if ( n1 == 0 && n2 == 0) return 0;       //both empty.. no different
        if ( n1 == 0 || n2 == 0) return 1000;    //one is empty inf difference

        float meanA = groupA.mean();
        float meanB = groupB.mean();

        float stdA  = groupA.stddev(meanA);
        float stdB  = groupB.stddev(meanB);
        if (stdA == 0 ) stdA=1.0;
        if (stdB == 0 ) stdB=1.0;

        float t_test = (meanA-meanB)/sqrt(((stdA*stdA)/n1)+((stdB*stdB)/n2));
        return t_test;
    }


    int countBelow(vector<float>& y, float ymax) {
        vector<float>::iterator itr = lower_bound(y.begin(), y.end(), ymax);
        int lb = itr-y.begin();
        return lb;
        /*
           int count=0;
           for (int i=0; i < y.size(); i++) if (y[i] < ymax) count++;
           return(count);
           */
    }

    bool fileExists(string strFilename) {
        struct stat stFileInfo;
        bool blnReturn;
        int intStat;
        // Attempt to get the file attributes
        intStat = stat(strFilename.c_str(),&stFileInfo);
        if(intStat == 0) {
            blnReturn = true;
        } else {
            blnReturn = false;
        }
        return(blnReturn);
    }


    int createDir(const char* path) {
        if (isDir(path)) return 0;
        cout << "Creating path: " << path << endl;
        mode_t old_mask = umask(0);
#ifdef MINGW
        int retval = mkdir(path);
#else
        int retval = mkdir(path, 0771);
#endif
        umask(old_mask);
        return retval;
    }

    int isFile(const char* path) {
        struct stat sbuf;
        int retval = stat(path, &sbuf);
        return (!retval && (sbuf.st_mode & S_IFREG));
    }

    int isDir(const char* path) {
        struct stat sbuf;
        int retval = stat(path, &sbuf);
        return (!retval && (sbuf.st_mode & S_IFDIR));
    }

    bool almostEqual(double a, double b)
    {
        // the machine epsilon has to be scaled to the magnitude of the values
        // used and multiplied by the desired precision in ULPs (units in the
        // last place).
        return std::abs(a - b) <= std::numeric_limits<double>::epsilon()
                * std::abs(a + b)
                * 2
                // unless the result is subnormal
                || std::abs(a - b) < std::numeric_limits<double>::min();
    }

    bool almostEqual(float a, float b)
    {
        // the machine epsilon has to be scaled to the magnitude of the values
        // used and multiplied by the desired precision in ULPs (units in the
        // last place).
        return std::abs(a - b) <= std::numeric_limits<float>::epsilon()
                * std::abs(a + b)
                * 1
                // unless the result is subnormal
                || std::abs(a - b) < std::numeric_limits<float>::min();
    }

    float correlation(const vector<float>&x, const vector<float>&y) {
        int n = x.size();
        double sumx = 0; 		//
        double sumy = 0;
        double sumxy =0;
        double x2 = 0;
        double y2 = 0;

        for (int i = 0; i < n; i++) {
            sumx += x[i];
            sumy += y[i];
            sumxy += x[i]*y[i];
            x2 += x[i]*x[i];
            y2 += y[i]*y[i];
        }
        if (n == 0) return 0;
        double var1 = x2-(sumx*sumx)/n;
        double var2 = y2-(sumy*sumy)/n;
        if ( var1 == 0 || var2 == 0 ) return 0;
        return (sumxy -( sumx*sumy)/n) / sqrt((x2-(sumx*sumx)/n)*(y2-(sumy*sumy)/n));
    }


    /*peak fitting function*/
    void gaussFit(const vector<float>&ycoord, float* sigma, float* R2) {

        float s = 20;
        float min_s = 0;
        float minR = 1e99;

        //find best fit
        if (ycoord.size()<3) return;
        vector<float>yobs=ycoord;
        int ysize=yobs.size();
        int midpoint  = int(ysize/2);
        //find maximum point ( assuming it somewhere around midpoint of the yobs);
        float ymax = max(max(yobs[midpoint], yobs[midpoint-1]),yobs[midpoint+1]);
        float ymin = min( yobs[0], yobs[ysize-1]);

        //initialize x vector, values centered around 0, forxample  -2, -1, 0, 1, 2
        int xinit = int(ysize/2)*-1;
        vector<float>x(ysize,0);
        int greaterZeroCount=0;

        for(int i=0; i<ysize; i++ ) {
            x[i] = xinit+i;
            if ( yobs[i] > ymin ) greaterZeroCount++;
            yobs[i] = (yobs[i]-ymin)/(ymax-ymin);
            if(yobs[i]<0) yobs[i]=0;
        }

        /*
           cerr << "fitting yobs:" << endl; for(int i=0; i < ysize; i++ )
           cerr << setprecision(2) << yobs[i] << ", "; cerr << endl;
           */

        bool converged = false;
        int ittr = 0;

        if (greaterZeroCount <= 3 ) return;
        while (!converged ) {
            if ( ittr++ > 20 ) break;
            float Rsqr=0;
            for(int i=0; i < ysize; i++ )  { Rsqr += POW2(exp(-0.5*POW2(x[i]/s)) - yobs[i]); }
            //       cerr << "\t\ts=" << s << " Rsqr=" << Rsqr << endl;
            if ( Rsqr < minR || ittr == 0 ) { minR = Rsqr; min_s = s; }
            else if ( Rsqr > minR ) break;
            else if ( Rsqr - minR == 0 ) break;
            s /= 1.25;
        }

        *sigma = min_s;
        *R2 = minR/(ysize*ysize);	//corrected R2
        //cerr << "fit() s=" << *sigma << " R2=" << *R2 << endl;
    }


    inline unsigned long factorial(int n) {
        long p=1; while(n>1) p*=n--; return p; }

    /*
       long nchoosek(int n, int k) {
       if(k==n) return 1;
       if(k==0) return 1;
       if(k>n) return 0;
       return (factorial(n)/(factorial(n-k)*factorial(k)));
       }
       */
    long long nchoosek(int n, int k) {

        int n_k = n - k;

        if (k < n_k)
        {
            k = n_k;
            n_k = n - k;
        }

        long long  nchsk = 1;
        for ( int i = 1; i <= n_k; i++)
        {
            nchsk *= (++k);
            nchsk /= i;
        }
        return nchsk;
    }

    string cleanFilename(const string& filename) {

        string outstring=filename;
        std::string::size_type pos =outstring.find_last_of("/");

        if (pos != std::string::npos) {
            outstring=outstring.substr(pos+1, outstring.length());
        }

        pos=outstring.find_last_of("\\");
        if (pos != std::string::npos) {
            outstring=outstring.substr(pos+1, outstring.length());
        }

        pos=outstring.find_last_of(".");
        if (pos != std::string::npos) {
            outstring=outstring.substr(0,pos);
        }
        return outstring;
    }

    std::vector<double> naturalAbundanceCorrection(int nC, std::vector<double>& M, std::map<unsigned int, string> carbonIsotopeSpecies) {
        //inputs
        int n = nC;		//number of carbonms
        int nr_13C = n + 1; //number of labeled carbons

        //uncorrected values
        vector<double>C(nr_13C,0); // output vector with corrected values
        vector<double>COriginal(carbonIsotopeSpecies.size(),0); // output vector with corrected values
        unsigned int carbonIsotopicSpeciesInt = 0;

        for(int k=0; k < nr_13C; k++ ) {
            double contamination=0;

            for(int i=0; i<k; i++ ) {
                contamination += pow( 0.011,k-i) * pow(0.989, n-k) * nchoosek(n-i,k-i) * C[i];
            }

            C[k] = (M[k]-contamination) / pow(0.989,n-k);
            if(C[k] < 1e-4) C[k]=0;

            if (carbonIsotopeSpecies.find(k) != carbonIsotopeSpecies.end()) {
                COriginal[carbonIsotopicSpeciesInt++] = C[k];
            }
        }

        return COriginal;
    }


    const long double 	Pi = 3.1415926535897932384626433832795028841968;

    double beta(double x, double y) {

        if (x >= 2 && y >= 2 ) { //approximation
            return sqrt(2*Pi)*pow(x,x-0.5)*pow(y,y-0.5)/pow(x+y,(x+y-0.5));
        }

        //integral form
        double dt=0.01;
        double sum=0;
        for(double t=0.0001; t<=0.9999; t+=dt) {
            sum += pow(t,(x-1))*pow((1-t),(y-1))*dt;
        }
        return sum;
    }

    double gamma(double z) {
        //integral form
        double dt=0.0001;
        double sum=0;
        for(double t=0.000001; t<=10; t+=dt) {
            sum += pow(t,z-1)*exp(-t)*dt;
        }
        return sum;
    }

    double betaPDF(double x, double a, double b) {
        return pow(x,a-1)*pow(1-x,b-1)/beta(a,b);
    }

    double pertPDF(double x, double min, double mode, double max ) {
        double a = 6*(mode-min)/(max-min);
        double b = 6*(max-mode)/(max-min);
        return pow(x-min,a-1)*pow(max-x,b-1)/(beta(a,b)*pow(max-min,a+b-1));
    }


    void tridiagonal ( int n, float *c, float *a, float *b, float *r )

    {
        int i;

        for ( i = 0; i < n-1; i++ ) {
            b[i] /= a[i];
            a[i+1] -= c[i]*b[i];
        }

        r[0] /= a[0];
        for ( i = 1; i < n; i++ )
            r[i] = ( r[i] - c[i-1] * r[i-1] ) / a[i];

        for ( i = n-2; i >= 0; i-- )
            r[i] -= r[i+1] * b[i];
    }


    void cubic_nak ( int n, float *x, float *f, float *b, float *c, float *d )

        /*
           PURPOSE:
           determine the coefficients for the 'not-a-knot'
           cubic spline for a given set of data


           CALLING SEQUENCE:
           cubic_nak ( n, x, f, b, c, d );


           INPUTS:
           n		number of interpolating points
           x		array containing interpolating points
           f		array containing function values to
           be interpolated;  f[i] is the function
           value corresponding to x[i]
           b		array of size at least n; contents will
           be overwritten
           c		array of size at least n; contents will
           be overwritten
           d		array of size at least n; contents will
           be overwritten


           OUTPUTS:
           b		coefficients of linear terms in cubic
           spline
           c		coefficients of quadratic terms in
           cubic spline
           d		coefficients of cubic terms in cubic
           spline

           REMARK:
           remember that the constant terms in the cubic spline
           are given by the function values being interpolated;
           i.e., the contents of the f array are the constant
           terms

           to evaluate the cubic spline, use the routine
           'spline_eval'
           */

    {
        float *h,
              *dl,
              *dd,
              *du;
        int i;

        h  = new float [n];
        dl = new float [n];
        dd = new float [n];
        du = new float [n];

        for ( i = 0; i < n-1; i++ )
            h[i] = x[i+1] - x[i];
        for ( i = 0; i < n-3; i++ )
            dl[i] = du[i] = h[i+1];

        for ( i = 0; i < n-2; i++ ) {
            dd[i] = 2.0 * ( h[i] + h[i+1] );
            c[i]  = ( 3.0 / h[i+1] ) * ( f[i+2] - f[i+1] ) -
                ( 3.0 / h[i] ) * ( f[i+1] - f[i] );
        }
        dd[0] += ( h[0] + h[0]*h[0] / h[1] );
        dd[n-3] += ( h[n-2] + h[n-2]*h[n-2] / h[n-3] );
        du[0] -= ( h[0]*h[0] / h[1] );
        dl[n-4] -= ( h[n-2]*h[n-2] / h[n-3] );

        tridiagonal ( n-2, dl, dd, du, c );

        for ( i = n-3; i >= 0; i-- )
            c[i+1] = c[i];
        c[0] = ( 1.0 + h[0] / h[1] ) * c[1] - h[0] / h[1] * c[2];
        c[n-1] = ( 1.0 + h[n-2] / h[n-3] ) * c[n-2] - h[n-2] / h[n-3] * c[n-3];
        for ( i = 0; i < n-1; i++ ) {
            d[i] = ( c[i+1] - c[i] ) / ( 3.0 * h[i] );
            b[i] = ( f[i+1] - f[i] ) / h[i] - h[i] * ( c[i+1] + 2.0*c[i] ) / 3.0;
        }

        delete [] h;
        delete [] du;
        delete [] dd;
        delete [] dl;
    }

    float spline_eval ( int n, float *x, float *f, float *b, float *c,
            float *d, float t )

        /*
           PURPOSE:
           evaluate a cubic spline at a single value of
           the independent variable given the coefficients of
           the cubic spline interpolant (obtained from
           'cubic_nak' or 'cubic_clamped')


           CALLING SEQUENCE:
           y = spline_eval ( n, x, f, b, c, d, t );
           spline_eval ( n, x, f, b, c, d, t );


           INPUTS:
           n		number of interpolating points
           x		array containing interpolating points
           f		array containing the constant terms from
           the cubic spline (obtained from 'cubic_nak'
           or 'cubic_clamped')
           b		array containing the coefficients of the
           linear terms from the cubic spline
           (obtained from 'cubic_nak' or 'cubic_clamped')
           c		array containing the coefficients of the
           quadratic terms from the cubic spline
           (obtained from 'cubic_nak' or 'cubic_clamped')
           d		array containing the coefficients of the
           cubic terms from the cubic spline
           (obtained from 'cubic_nak' or 'cubic_clamped')
           t		value of independent variable at which
           the interpolating polynomial is to be
           evaluated


           OUTPUTS:
           y		value of cubic spline at the specified
           value of the independent variable
           */

    {
        int i=1;
        int found=0;

        while ( !found && ( i < n-1 ) ) {
            if ( t < x[i] )
                found = 1;
            else
                i++;
        }

        t = f[i-1] + ( t - x[i-1] ) * ( b[i-1] + ( t - x[i-1] ) * ( c[i-1] + (t - x[i-1] ) * d[i-1] ) );
        return ( t );
    }


    /** Decompress an STL string using zlib and return the original data. */
    std::string decompress_string(const std::string& str)
    {
        std::string outstring;

#ifdef ZLIB
        z_stream zs;                        // z_stream is zlib's control structure
        memset(&zs, 0, sizeof(zs));



        if (inflateInit(&zs) != Z_OK)
            throw(std::runtime_error("inflateInit failed while decompressing."));

        zs.next_in = (Bytef*)str.data();
        zs.avail_in = str.size();

        int ret;
        char outbuffer[32768];

        // get the decompressed bytes blockwise using repeated calls to inflate
        do {
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
            zs.avail_out = sizeof(outbuffer);

            ret = inflate(&zs, Z_NO_FLUSH);

            if (outstring.size() < zs.total_out) {
                outstring.append(outbuffer,
                        zs.total_out - outstring.size());
            }

        } while (ret == Z_OK);

        inflateEnd(&zs);

        if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
            std::ostringstream oss;
            oss << "Exception during zlib decompression: (" << ret << ") "
                << zs.msg;
            // throw(std::runtime_error(oss.str()));
        }

#endif
        return outstring;
    }

    bool gzipInflate( const std::string& compressedBytes, std::string& uncompressedBytes ) {
#ifdef ZLIB
        if ( compressedBytes.size() == 0 ) {
            uncompressedBytes = compressedBytes ;
            return true ;
        }

        uncompressedBytes.clear() ;

        unsigned full_length = compressedBytes.size() ;
        unsigned half_length = compressedBytes.size() / 2;

        unsigned uncompLength = full_length ;
        char* uncomp = (char*) calloc( sizeof(char), uncompLength );

        z_stream strm;
        strm.next_in = (Bytef *) compressedBytes.c_str();
        strm.avail_in = compressedBytes.size() ;
        strm.total_out = 0;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;

        bool done = false ;

        if (inflateInit2(&strm, (16+MAX_WBITS)) != Z_OK) {
            free( uncomp );
            return false;
        }

        while (!done) {
            // If our output buffer is too small
            if (strm.total_out >= uncompLength ) {
                // Increase size of output buffer
                char* uncomp2 = (char*) calloc( sizeof(char), uncompLength + half_length );
                memcpy( uncomp2, uncomp, uncompLength );
                uncompLength += half_length ;
                free( uncomp );
                uncomp = uncomp2 ;
            }

            strm.next_out = (Bytef *) (uncomp + strm.total_out);
            strm.avail_out = uncompLength - strm.total_out;

            // Inflate another chunk.
            int err = inflate (&strm, Z_SYNC_FLUSH);
            if (err == Z_STREAM_END) done = true;
            else if (err != Z_OK)  {
                break;
            }
        }

        if (inflateEnd (&strm) != Z_OK) {
            free( uncomp );
            return false;
        }

        for ( size_t i=0; i<strm.total_out; ++i ) {
            uncompressedBytes += uncomp[ i ];
        }
        free( uncomp );
#endif
        return true ;
    }

    bool strcasecmp_withNumbers(const std::string& a, const std::string& b ) {
        if (a.empty())
            return true;
        if (b.empty())
            return false;
        if (std::isdigit(a[0]) && !std::isdigit(b[0]))
            return true;
        if (!std::isdigit(a[0]) && std::isdigit(b[0]))
            return false;
        if (!std::isdigit(a[0]) && !std::isdigit(b[0])) {
            if (std::toupper(a[0]) == std::toupper(b[0]))
                return strcasecmp_withNumbers(a.substr(1), b.substr(1));
            return (std::toupper(a[0]) < std::toupper(b[0]));
        }

        // Both strings begin with digit --> parse both numbers
        std::istringstream issa(a);
        std::istringstream issb(b);
        int ia, ib;
        issa >> ia;
        issb >> ib;
        if (ia != ib)
            return ia < ib;

        // Numbers are the same --> remove numbers and recurse
        std::string anew, bnew;
        std::getline(issa, anew);
        std::getline(issb, bnew);
        return (strcasecmp_withNumbers(anew, bnew));
    }

} //namespace end
