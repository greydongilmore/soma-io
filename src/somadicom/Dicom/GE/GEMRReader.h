#ifndef _GEMRReader_h_
#define _GEMRReader_h_


#include <soma-io/config/soma_config.h>
#include <soma-io/Dicom/MRImageStorageReader.h>
#include <soma-io/Pattern/Singleton.h>


class DcmDataset;


namespace soma
{


class GEMRReader : public MRImageStorageReader,
                   public Singleton< GEMRReader >
{

  public:

    std::string getManufacturerName();

    virtual bool getHeader( HeaderProxy& header, 
                            DataInfo& info,
                            DicomDatasetHeader& datasetHeader );

  protected:

    friend class Singleton< GEMRReader >;

    GEMRReader();
    ~GEMRReader();

};


}


#endif
