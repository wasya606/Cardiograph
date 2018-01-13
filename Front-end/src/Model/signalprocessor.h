#ifndef SIGNALPROCESSOR_H
#define SIGNALPROCESSOR_H
#include <complex>
#include <vector>
#include <QVariantList>

using namespace std;

#define MAXN 1024

using base = complex<double>;

class SignalProcessor
{
public:
    SignalProcessor();

    void fft(QVariantList& buffer);

private:
    void fft (base a[], int n, bool invert);
    void calc_rev (int n, int log_n);

private:
    int rev[MAXN];
    base wlen_pw[MAXN];

};

#endif // SIGNALPROCESSOR_H
