#ifndef ___BWT
#define ___BWT
#include "transform.h"
class Bwt: public Transform
{
    //Similar to RLE, refer to its formatting
    void transform(vector<unique_ptr<Block> > &input) override;
    void applyTo(vector<long> &data) override;
    void decode(vector<unique_ptr<Block> > &input) override;
    void deplyTo(vector<long> &data);

public:
    Bwt(Transform *next);
};

#endif