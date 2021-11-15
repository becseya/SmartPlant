#pragma once

#include "Aggregator.hpp"

#include <limits>

#define FLOAT_MAX std::numeric_limits<float>::max();
#define FLOAT_MIN std::numeric_limits<float>::min();

namespace SmartPlant {
namespace Aggregation {

class ScalarAggregator : public Aggregator
{
  public:
    ScalarAggregator(const char* name, bool printAverage)
        : Aggregator(name)
        , printAverage(printAverage)
    {
        reset();
    }

    void addSample(float sample)
    {
        sum += sample;
        samples++;
        // TODO 1: add update logic for min/max
    }

    void  update() 
    {

    }

    void reset() final
    {
        sum     = 0;
        samples = 0;
        // TODO 1: add reset logic for min/max (use FLOAT_MIN, FLOAT_MAX macros)
      
        printf("enter the number of samples:"); 
        scanf("%d", &samples);   
        max=samples;                                   
        min=samples;    
        do
        {
            if(samples!=-1)
            {
                if(samples>max)
                
                     max=samples;  
                
                else 
                if (samples<min) 
                
                    min=samples;
                    printf("enter the number of samples:"); 
                    scanf("%d", &samples);                           
            }
        }
            while(samples!=-1);       
            printf("the maximum %.6f is : ", max);
            printf("the minimum %.6f is ", min);                    
    }





    void printResult() final
    {
        if (printAverage)
            LOG_AGGREGATOR("Min: %.1f, Max: %.1f, Avg.: %.1f", min, max, sum / samples)
        else
            LOG_AGGREGATOR("Min: %.1f, Max: %.1f", min, max)
    }

  protected:
    const bool printAverage;
    float      sum;
    float      min;
    float      max;
    int        samples;
};

} // namespace Aggregation
} // namespace SmartPlant

// TODO 2: Move functions bodies to proper .cpp files
