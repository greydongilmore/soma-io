/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

#ifndef SOMAIO_IO_SCALEDCODING_H
#define SOMAIO_IO_SCALEDCODING_H

#include <soma-io/config/soma_config.h>
#include <vector>

namespace carto
{
  template <typename T> class Volume;
}

template <typename T> class AimsData;


namespace soma
{

  /** Checks if a volume can be encoded as 16 bit signed ints with a scale
      factor and optionally an offset. If so, the slope and offset are
      determined and set.
  */
  template <typename T>
  bool canEncodeAsScaledS16( const T* vol, float & slope,
                             float & offset,
                             const std::vector<long> & strides,
                             const std::vector<int> & sizes,
                             bool enableoffset = true,
                             double *maxerr = 0 );

  // declare specializations that actually do something
  template <>
  bool canEncodeAsScaledS16( const float* vol, float & slope,
                             float & offset,
                             const std::vector<long> & strides,
                             const std::vector<int> & sizes,
                             bool enableoffset, double * );
  template <>
  bool canEncodeAsScaledS16( const double* vol, float & slope,
                             float & offset,
                             const std::vector<long> & strides,
                             const std::vector<int> & sizes,
                             bool enableoffset, double * );

  // default implementation always fails
  template <typename T>
  inline bool canEncodeAsScaledS16( const T*, float &,
                                    float &,
                                    const std::vector<long> &,
                                    const std::vector<int> &,
                                    bool, double * )
  {
    return false;
  }

  class ScaledEncodingInfo
  {
    public :
      ScaledEncodingInfo() : 
        _slope(1), _offset(0), _maxerr(0) {}

      virtual ~ScaledEncodingInfo() {}

      double & slope() { return _slope; }
      double & offset() { return _offset; }
      double & maxerr() { return _maxerr; }

    private :
      double _slope;
      double _offset;
      double _maxerr;
  };

  template <typename INP, typename OUTP>
  class ScaledEncoding
  {
    public :

      /** Get the slope, offset and maximum error to encode a volume of float
          or double using an integer type.
      */
      static ScaledEncodingInfo info( const INP * thing,
                                      const std::vector<long> & strides,
                                      const std::vector<int> & sizes );

      static ScaledEncodingInfo rescale( const INP * in,
                                         const std::vector<long> & strides,
                                         const std::vector<int> & sizes,
                                         OUTP * out );
  };
}

#endif



