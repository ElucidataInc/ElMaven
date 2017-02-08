#include <math.h>

namespace mzUtils
{

    template <typename T>  inline T __min (T a, T b)
    {
        if (a<b) return a ;
        return b ;
    }

    const float TINY = 1.0e-20f ;

    int ludcmp(float **a, int n, int *indx, float *d)
    {
        int i,imax,j,k;
        imax = 0;
        float big=0,dum=0,sum=0,temp=0;
        float *vv = new float [n] ;
        for (j = 0 ; j < n ; j++)
        {
            vv[j] = 0 ;
        }


        *d=1.0;
        for (i=1;i<=n;i++) {
            big=0.0;
            for (j=1;j<=n;j++)
                if ((temp=fabs(a[i-1][j-1])) > big) big=temp;
            if (big == 0.0)
            {
                delete [] vv ;
                return -1 ;
            }
            vv[i-1]= 1.0f/big;
        }
        for (j=1;j<=n;j++) {
            for (i=1;i<j;i++) {
                sum=a[i-1][j-1];
                for (k=1;k<i;k++) sum -= a[i-1][k-1]*a[k-1][j-1];
                a[i-1][j-1]=sum;
            }
            big=0.0;
            for (i=j;i<=n;i++) {
                sum=a[i-1][j-1];
                for (k=1;k<j;k++)
                    sum -= a[i-1][k-1]*a[k-1][j-1];
                a[i-1][j-1]=sum;
                if ( (dum=vv[i-1]*fabs(sum)) >= big) {
                    big=dum;
                    imax=i;
                }
            }
            if (j != imax) {
                for (k=1;k<=n;k++) {
                    dum=a[imax-1][k-1];
                    a[imax-1][k-1]=a[j-1][k-1];
                    a[j-1][k-1]=dum;
                }
                *d = -(*d);
                vv[imax-1]=vv[j-1];
            }
            indx[j-1]=imax;
            if (a[j-1][j-1] == 0.0) a[j-1][j-1]=TINY;
            if (j != n) {
                dum=1.0f/(a[j-1][j-1]);
                for (i=j+1;i<=n;i++) a[i-1][j-1] *= dum;
            }
        }
        delete [] vv ;
        return 0 ;
    }

#undef TINY

    void lubksb(float **a, int n,int *indx, float b[] )
    {
        int i=0,ii=0,ip=0,j=0;
        float sum=0;

        for (i=1;i<=n;i++) {
            ip=indx[i-1];
            sum=b[ip-1];
            b[ip-1]=b[i-1];
            if (ii)
                for (j=ii;j<=i-1;j++) sum -= a[i-1][j-1]*b[j-1];
            else if (sum) ii=i;
            b[i-1]=sum;
        }
        for (i=n;i>=1;i--) {
            sum=b[i-1];
            for (j=i+1;j<=n;j++) sum -= a[i-1][j-1]*b[j-1];
            b[i-1]=sum/a[i-1][i-1];
        }
    }


    int savgol(float *c, int np, int nl,int nr, int ld, int m)
    {
        int mm=0,*indx;
        float d=0,fac=0,sum=0, **a,*b;

        if(np < nl+nr+1 || nl < 0 || nr < 0 || ld > m || nl+nr < m) return(-1);
        indx= new int [m+1] ;
        b= new float [m+1] ;
        for (int j = 0 ; j < m+1 ; j++)
        {
            indx[j] = 0 ;
            b[j] = 0 ;
        }

        a = new float* [m+1] ;
        for (int j = 0 ; j < m+1 ; j++)
        {
            a[j] = new float [m+1] ;
            for (int i = 0 ; i < m+1 ; i++)
            {
                a[j][i] = 0 ;
            }
        }


        for(int ipj=0;ipj<=(m<<1);ipj++)
        {
            sum = (ipj ? 0.0f : 1.0f);
            for(int k=1;k<=nr;k++) sum += (float) pow((double)k,(double)ipj);
            for(int k=1;k<=nl;k++) sum += (float) pow((double)-k,(double)ipj);
            mm=__min(ipj,2*m-ipj);
            for(int imj = -mm;imj<=mm;imj+=2) a[(ipj+imj)/2][(ipj-imj)/2]=sum;
        }
        int ret_val = ludcmp(a,m+1,indx,&d) ;
        if (ret_val == -1)
        {
            delete [] indx ;
            delete [] b;

            for (int j = 0 ; j < m+1 ; j++)
            {
                delete [] a[j] ;
            }
            delete [] a ;
            for(int kk=1;kk<=np;kk++)
                c[kk]=0.0;
            return -1 ;
        }

        for(int j=1;j<=m+1;j++) b[j-1]=0.0;
        b[ld]=1.0;
        lubksb(a,m+1,indx,b);

        for(int kk=1;kk<=np;kk++) c[kk]=0.0;

        for(int k=-nl;k<=nr;k++)
        {
            sum=b[0];
            fac=1.0;
            for(mm=1;mm<=m;mm++) sum += b[mm]*(fac *= k);
            int kk=((np-k) % np)+1;
            c[kk]=sum;
        }

        delete [] indx ;
        delete [] b;

        for (int j = 0 ; j < m+1 ; j++)
        {
            delete [] a[j] ;
        }
        delete [] a ;
        return 0;
        
    }
}
