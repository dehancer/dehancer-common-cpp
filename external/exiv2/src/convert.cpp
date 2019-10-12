// ***************************************************************** -*- C++ -*-
/*
 * Copyright (C) 2004-2018 Exiv2 authors
 * This program is part of the Exiv2 distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA.
 */
/*
  File:      convert.cpp
  Author(s): Andreas Huggel (ahu) <ahuggel@gmx.net>
             Vladimir Nadvornik (vn) <nadvornik@suse.cz>
  History:   17-Mar-08, ahu: created basic converter framework
             20-May-08, vn:  added actual conversion logic
 */
// *****************************************************************************
// included header files
#include "config.h"
#include "types.hpp"
#include "error.hpp"
#include "exif.hpp"
#include "iptc.hpp"
#include "xmp_exiv2.hpp"
#include "futils.hpp"
#include "convert.hpp"
#include "unused.h"

// + standard includes
#include <utility>
#include <iostream>
#include <iomanip>
#include <ios>
#include <sstream>
#include <stdio.h> // for snprintf (C99)
#ifdef _MSC_VER
# define snprintf _snprintf
#endif
#include <cstring>

#if defined WIN32 && !defined __CYGWIN__
# include <windows.h>
#endif

#ifdef EXV_HAVE_ICONV
# include <iconv.h>
# include <errno.h>
#endif

// Adobe XMP Toolkit
#ifdef EXV_HAVE_XMP_TOOLKIT
# define TXMP_STRING_TYPE std::string
# ifdef EXV_ADOBE_XMPSDK
#  include <XMP.hpp>
# else
#  include <XMPSDK.hpp>
# endif
# include <MD5.h>
#endif // EXV_HAVE_XMP_TOOLKIT

// *****************************************************************************
// local declarations
namespace {
#if defined WIN32 && !defined __CYGWIN__
    // Convert string charset with Windows functions.
    bool convertStringCharsetWindows(std::string& str, const char* from, const char* to);
#endif
#if defined EXV_HAVE_ICONV
    // Convert string charset with iconv.
    bool convertStringCharsetIconv(std::string& str, const char* from, const char* to);
#endif
    /*!
      @brief Get the text value of an XmpDatum \em pos.

      If \em pos refers to a LangAltValue, \em value is set to the default language
      entry without the x-default qualifier. If there is no default but
      exactly one entry, \em value is set to this entry, without the qualifier.
      The return code indicates if the operation was successful.
     */
    bool getTextValue(std::string& value, const Exiv2::XmpData::iterator& pos);
}

// *****************************************************************************
// class member definitions
namespace Exiv2 {

    //! Metadata conversions.
    class Converter {
    public:
        /*!
          @brief Type for metadata converter functions, taking two key strings,
                 \em from and \em to.

          These functions have access to both the source and destination metadata
          containers and store the result directly in the destination container.
         */
        typedef void (Converter::*ConvertFct)(const char* from, const char* to);
        //! Structure to define conversions between two keys.
        struct Conversion {
            MetadataId  metadataId_; //!< Type of metadata for the first key.
            const char* key1_;       //!< First metadata key.
            const char* key2_;       //!< Second metadata key (always an XMP key for now).
            ConvertFct  key1ToKey2_; //!< Conversion from first to second key.
            ConvertFct  key2ToKey1_; //!< Conversion from second to first key.
        };
    public:
        //! @name Creators
        //@{
        //! Constructor for Exif tags and XMP properties.
        Converter(ExifData& exifData, XmpData& xmpData);
        //! Constructor for Iptc tags and XMP properties.
        Converter(IptcData& iptcData, XmpData& xmpData, const char *iptcCharset = 0);
        //@}

        //! @name Manipulators
        //@{
        //! Convert Exif tags or IPTC datasets to XMP properties according to the conversion table.
        void cnvToXmp();
        //! Convert XMP properties to Exif tags or IPTC datasets according to the conversion table.
        void cnvFromXmp();
        /*!
          @brief Set the erase flag.

          This flag indicates whether successfully converted source records are erased.
         */
        void setErase(bool onoff =true) { erase_ = onoff; }
        /*!
          @brief Set the overwrite flag.

          This flag indicates whether existing target records are overwritten.
         */
        void setOverwrite(bool onoff =true) { overwrite_ = onoff; }
        //@}

        //! @name Conversion functions (manipulators)
        //@{
        /*!
          @brief Do nothing conversion function.

          Use when, for example, a one-way conversion is needed.
         */
        void cnvNone(const char*, const char*);
        /*!
          @brief Simple Exif to XMP conversion function.

          Sets the XMP property to an XmpText value containing the Exif value string.
         */
        void cnvExifValue(const char* from, const char* to);
        /*!
          @brief Convert the tag Exif.Photo.UserComment to XMP.

          Todo: Convert the Exif comment to UTF-8 if necessary.
         */
        void cnvExifComment(const char* from, const char* to);
        /*!
          @brief Converts Exif tag with multiple components to XMP array.

          Converts Exif tag with multiple components to XMP array. This function is
          used for ComponentsConfiguration tag.
         */
        void cnvExifArray(const char* from, const char* to);
        /*!
          @brief Exif date to XMP conversion function.

          Sets the XMP property to an XmpText value containing date and time. This function
          combines values from multiple Exif tags as described in XMP specification. It
          is used for DateTime, DateTimeOriginal, DateTimeDigitized and GPSTimeStamp.
         */
        void cnvExifDate(const char* from, const char* to);
        /*!
          @brief Exif version to XMP conversion function.

          Converts ExifVersion tag to XmpText value.
         */
        void cnvExifVersion(const char* from, const char* to);
        /*!
          @brief Exif GPS version to XMP conversion function.

          Converts GPSVersionID tag to XmpText value.
         */
        void cnvExifGPSVersion(const char* from, const char* to);
        /*!
          @brief Exif Flash to XMP conversion function.

          Converts Flash tag to XMP structure.
         */
        void cnvExifFlash(const char* from, const char* to);
        /*!
          @brief Exif GPS coordinate to XMP conversion function.

          Converts GPS coordinates tag to XmpText value. It combines multiple Exif tags
          as described in XMP specification.
         */
        void cnvExifGPSCoord(const char* from, const char* to);
        /*!
          @brief Simple XMP to Exif conversion function.

          Sets the Exif tag according to the XMP property.
          For LangAlt values, only the x-default entry is used.

          Todo: Escape non-ASCII characters in XMP text values
         */
        void cnvXmpValue(const char* from, const char* to);
        /*!
          @brief Convert the tag Xmp.exif.UserComment to Exif.
         */
        void cnvXmpComment(const char* from, const char* to);
        /*!
          @brief Converts XMP array to Exif tag with multiple components.

          Converts XMP array to Exif tag with multiple components. This function is
          used for ComponentsConfiguration tag.
         */
        void cnvXmpArray(const char* from, const char* to);
        /*!
          @brief XMP to Exif date conversion function.

          Converts the XmpText value to Exif date and time. This function
          sets multiple Exif tags as described in XMP specification. It
          is used for DateTime, DateTimeOriginal, DateTimeDigitized and GPSTimeStamp.
         */
        void cnvXmpDate(const char* from, const char* to);
        /*!
          @brief XMP to Exif version conversion function.

          Converts XmpText value to ExifVersion tag.
         */
        void cnvXmpVersion(const char* from, const char* to);
        /*!
          @brief XMP to Exif GPS version conversion function.

          Converts XmpText value to GPSVersionID tag.
         */
        void cnvXmpGPSVersion(const char* from, const char* to);
        /*!
          @brief XMP to Exif Flash conversion function.

          Converts XMP structure to Flash tag.
         */
        void cnvXmpFlash(const char* from, const char* to);
        /*!
          @brief XMP to Exif GPS coordinate conversion function.

          Converts XmpText value to GPS coordinates tags. It sets multiple Exif tags
          as described in XMP specification.
         */
        void cnvXmpGPSCoord(const char* from, const char* to);
        /*!
          @brief IPTC dataset to XMP conversion function.

          Multiple IPTC datasets with the same key are converted to an XMP array.
         */
        void cnvIptcValue(const char* from, const char* to);
        /*!
          @brief XMP to IPTC dataset conversion function.

          Each array element of an XMP array value is added as one IPTC dataset.
         */
        void cnvXmpValueToIptc(const char* from, const char* to);
        /*!
          @brief Write exif:NativeDigest and tiff:NativeDigest properties to XMP.

          Compute digests from Exif values and write them to  exif:NativeDigest
          and tiff:NativeDigest properties. This should be compatible with XMP SDK.
         */
        void writeExifDigest();
        /*!
          @brief Copies metadata in appropriate direction.

          From values of exif:NativeDigest and tiff:NativeDigest detects which of
          XMP and Exif was updated more recently and copies metadata in appropriate direction.
         */
        void syncExifWithXmp();
        //@}

        //! @name Accessors
        //@{
        //! Get the value of the erase flag, see also setErase(bool on).
        bool erase() const { return erase_; }
        //! Get the value of the overwrite flag, see also setOverwrite(bool on).
        bool overwrite() const { return overwrite_; }
        //@}

    private:
        bool prepareExifTarget(const char* to, bool force =false);
        bool prepareIptcTarget(const char* to, bool force =false);
        bool prepareXmpTarget(const char* to, bool force =false);
        std::string computeExifDigest(bool tiff);

        // DATA
        static const Conversion conversion_[];  //<! Conversion rules
        bool erase_;
        bool overwrite_;
        ExifData *exifData_;
        IptcData *iptcData_;
        XmpData  *xmpData_;
        const char *iptcCharset_;

    }; // class Converter

    // Order is important for computing digests
    const Converter::Conversion Converter::conversion_[] = {
        { mdExif, "Exif.Image.ImageWidth",                "MLutXmp.tiff.ImageWidth",                &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.ImageLength",               "MLutXmp.tiff.ImageLength",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.BitsPerSample",             "MLutXmp.tiff.BitsPerSample",             &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.Compression",               "MLutXmp.tiff.Compression",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.PhotometricInterpretation", "MLutXmp.tiff.PhotometricInterpretation", &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.Orientation",               "MLutXmp.tiff.Orientation",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.SamplesPerPixel",           "MLutXmp.tiff.SamplesPerPixel",           &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.PlanarConfiguration",       "MLutXmp.tiff.PlanarConfiguration",       &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.YCbCrSubSampling",          "MLutXmp.tiff.YCbCrSubSampling",          &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.YCbCrPositioning",          "MLutXmp.tiff.YCbCrPositioning",          &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.XResolution",               "MLutXmp.tiff.XResolution",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.YResolution",               "MLutXmp.tiff.YResolution",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.ResolutionUnit",            "MLutXmp.tiff.ResolutionUnit",            &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.TransferFunction",          "MLutXmp.tiff.TransferFunction",          &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.WhitePoint",                "MLutXmp.tiff.WhitePoint",                &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.PrimaryChromaticities",     "MLutXmp.tiff.PrimaryChromaticities",     &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.YCbCrCoefficients",         "MLutXmp.tiff.YCbCrCoefficients",         &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.ReferenceBlackWhite",       "MLutXmp.tiff.ReferenceBlackWhite",       &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.DateTime",                  "MLutXmp.mlut_xmp.ModifyDate",                 &Converter::cnvExifDate , &Converter::cnvXmpDate  }, // MWG Guidelines
        { mdExif, "Exif.Image.ImageDescription",          "MLutXmp.dc.description",                 &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.Make",                      "MLutXmp.tiff.Make",                      &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.Model",                     "MLutXmp.tiff.Model",                     &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.Software",                  "MLutXmp.tiff.Software",                  &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.Artist",                    "MLutXmp.dc.creator",                     &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.Rating",                    "MLutXmp.mlut_xmp.Rating",                     &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Image.Copyright",                 "MLutXmp.dc.rights",                      &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.ExifVersion",               "MLutXmp.exif.ExifVersion",               &Converter::cnvExifVersion, &Converter::cnvXmpVersion },
        { mdExif, "Exif.Photo.FlashpixVersion",           "MLutXmp.exif.FlashpixVersion",           &Converter::cnvExifVersion, &Converter::cnvXmpVersion },
        { mdExif, "Exif.Photo.ColorSpace",                "MLutXmp.exif.ColorSpace",                &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.ComponentsConfiguration",   "MLutXmp.exif.ComponentsConfiguration",   &Converter::cnvExifArray, &Converter::cnvXmpArray },
        { mdExif, "Exif.Photo.CompressedBitsPerPixel",    "MLutXmp.exif.CompressedBitsPerPixel",    &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.PixelXDimension",           "MLutXmp.exif.PixelXDimension",           &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.PixelYDimension",           "MLutXmp.exif.PixelYDimension",           &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.UserComment",               "MLutXmp.exif.UserComment",               &Converter::cnvExifComment, &Converter::cnvXmpComment },
        { mdExif, "Exif.Photo.RelatedSoundFile",          "MLutXmp.exif.RelatedSoundFile",          &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.DateTimeOriginal",          "MLutXmp.photoshop.DateCreated",          &Converter::cnvExifDate,  &Converter::cnvXmpDate  }, // MWG Guidelines
        { mdExif, "Exif.Photo.DateTimeDigitized",         "MLutXmp.mlut_xmp.CreateDate",                 &Converter::cnvExifDate,  &Converter::cnvXmpDate  }, // MWG Guidelines
        { mdExif, "Exif.Photo.ExposureTime",              "MLutXmp.exif.ExposureTime",              &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.FNumber",                   "MLutXmp.exif.FNumber",                   &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.ExposureProgram",           "MLutXmp.exif.ExposureProgram",           &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.SpectralSensitivity",       "MLutXmp.exif.SpectralSensitivity",       &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.ISOSpeedRatings",           "MLutXmp.exif.ISOSpeedRatings",           &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.OECF",                      "MLutXmp.exif.OECF",                      &Converter::cnvExifValue, &Converter::cnvXmpValue }, // FIXME ?
        { mdExif, "Exif.Photo.ShutterSpeedValue",         "MLutXmp.exif.ShutterSpeedValue",         &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.ApertureValue",             "MLutXmp.exif.ApertureValue",             &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.BrightnessValue",           "MLutXmp.exif.BrightnessValue",           &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.ExposureBiasValue",         "MLutXmp.exif.ExposureBiasValue",         &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.MaxApertureValue",          "MLutXmp.exif.MaxApertureValue",          &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.SubjectDistance",           "MLutXmp.exif.SubjectDistance",           &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.MeteringMode",              "MLutXmp.exif.MeteringMode",              &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.LightSource",               "MLutXmp.exif.LightSource",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.Flash",                     "MLutXmp.exif.Flash",                     &Converter::cnvExifFlash, &Converter::cnvXmpFlash },
        { mdExif, "Exif.Photo.FocalLength",               "MLutXmp.exif.FocalLength",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.SubjectArea",               "MLutXmp.exif.SubjectArea",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.FlashEnergy",               "MLutXmp.exif.FlashEnergy",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.SpatialFrequencyResponse",  "MLutXmp.exif.SpatialFrequencyResponse",  &Converter::cnvExifValue, &Converter::cnvXmpValue }, // FIXME ?
        { mdExif, "Exif.Photo.FocalPlaneXResolution",     "MLutXmp.exif.FocalPlaneXResolution",     &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.FocalPlaneYResolution",     "MLutXmp.exif.FocalPlaneYResolution",     &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.FocalPlaneResolutionUnit",  "MLutXmp.exif.FocalPlaneResolutionUnit",  &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.SubjectLocation",           "MLutXmp.exif.SubjectLocation",           &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.ExposureIndex",             "MLutXmp.exif.ExposureIndex",             &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.SensingMethod",             "MLutXmp.exif.SensingMethod",             &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.FileSource",                "MLutXmp.exif.FileSource",                &Converter::cnvExifValue, &Converter::cnvXmpValue }, // FIXME ?
        { mdExif, "Exif.Photo.SceneType",                 "MLutXmp.exif.SceneType",                 &Converter::cnvExifValue, &Converter::cnvXmpValue }, // FIXME ?
        { mdExif, "Exif.Photo.CFAPattern",                "MLutXmp.exif.CFAPattern",                &Converter::cnvExifValue, &Converter::cnvXmpValue }, // FIXME ?
        { mdExif, "Exif.Photo.CustomRendered",            "MLutXmp.exif.CustomRendered",            &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.ExposureMode",              "MLutXmp.exif.ExposureMode",              &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.WhiteBalance",              "MLutXmp.exif.WhiteBalance",              &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.DigitalZoomRatio",          "MLutXmp.exif.DigitalZoomRatio",          &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.FocalLengthIn35mmFilm",     "MLutXmp.exif.FocalLengthIn35mmFilm",     &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.SceneCaptureType",          "MLutXmp.exif.SceneCaptureType",          &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.GainControl",               "MLutXmp.exif.GainControl",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.Contrast",                  "MLutXmp.exif.Contrast",                  &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.Saturation",                "MLutXmp.exif.Saturation",                &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.Sharpness",                 "MLutXmp.exif.Sharpness",                 &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.DeviceSettingDescription",  "MLutXmp.exif.DeviceSettingDescription",  &Converter::cnvExifValue, &Converter::cnvXmpValue }, // FIXME ?
        { mdExif, "Exif.Photo.SubjectDistanceRange",      "MLutXmp.exif.SubjectDistanceRange",      &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.Photo.ImageUniqueID",             "MLutXmp.exif.ImageUniqueID",             &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSVersionID",            "MLutXmp.exif.GPSVersionID",              &Converter::cnvExifGPSVersion, &Converter::cnvXmpGPSVersion },
        { mdExif, "Exif.GPSInfo.GPSLatitude",             "MLutXmp.exif.GPSLatitude",               &Converter::cnvExifGPSCoord, &Converter::cnvXmpGPSCoord },
        { mdExif, "Exif.GPSInfo.GPSLongitude",            "MLutXmp.exif.GPSLongitude",              &Converter::cnvExifGPSCoord, &Converter::cnvXmpGPSCoord },
        { mdExif, "Exif.GPSInfo.GPSAltitudeRef",          "MLutXmp.exif.GPSAltitudeRef",            &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSAltitude",             "MLutXmp.exif.GPSAltitude",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSTimeStamp",            "MLutXmp.exif.GPSTimeStamp",              &Converter::cnvExifDate,  &Converter::cnvXmpDate  }, // FIXME ?
        { mdExif, "Exif.GPSInfo.GPSSatellites",           "MLutXmp.exif.GPSSatellites",             &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSStatus",               "MLutXmp.exif.GPSStatus",                 &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSMeasureMode",          "MLutXmp.exif.GPSMeasureMode",            &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSDOP",                  "MLutXmp.exif.GPSDOP",                    &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSSpeedRef",             "MLutXmp.exif.GPSSpeedRef",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSSpeed",                "MLutXmp.exif.GPSSpeed",                  &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSTrackRef",             "MLutXmp.exif.GPSTrackRef",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSTrack",                "MLutXmp.exif.GPSTrack",                  &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSImgDirectionRef",      "MLutXmp.exif.GPSImgDirectionRef",        &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSImgDirection",         "MLutXmp.exif.GPSImgDirection",           &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSMapDatum",             "MLutXmp.exif.GPSMapDatum",               &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSDestLatitude",         "MLutXmp.exif.GPSDestLatitude",           &Converter::cnvExifGPSCoord, &Converter::cnvXmpGPSCoord },
        { mdExif, "Exif.GPSInfo.GPSDestLongitude",        "MLutXmp.exif.GPSDestLongitude",          &Converter::cnvExifGPSCoord, &Converter::cnvXmpGPSCoord },
        { mdExif, "Exif.GPSInfo.GPSDestBearingRef",       "MLutXmp.exif.GPSDestBearingRef",         &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSDestBearing",          "MLutXmp.exif.GPSDestBearing",            &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSDestDistanceRef",      "MLutXmp.exif.GPSDestDistanceRef",        &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSDestDistance",         "MLutXmp.exif.GPSDestDistance",           &Converter::cnvExifValue, &Converter::cnvXmpValue },
        { mdExif, "Exif.GPSInfo.GPSProcessingMethod",     "MLutXmp.exif.GPSProcessingMethod",       &Converter::cnvExifValue, &Converter::cnvXmpValue }, // FIXME ?
        { mdExif, "Exif.GPSInfo.GPSAreaInformation",      "MLutXmp.exif.GPSAreaInformation",        &Converter::cnvExifValue, &Converter::cnvXmpValue }, // FIXME ?
        { mdExif, "Exif.GPSInfo.GPSDifferential",         "MLutXmp.exif.GPSDifferential",           &Converter::cnvExifValue, &Converter::cnvXmpValue },

        { mdIptc, "Iptc.Application2.ObjectName",         "MLutXmp.dc.title",                       &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.Urgency",            "MLutXmp.photoshop.Urgency",              &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.Category",           "MLutXmp.photoshop.Category",             &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.SuppCategory",       "MLutXmp.photoshop.SupplementalCategories", &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.Keywords",           "MLutXmp.dc.subject",                     &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.SubLocation",        "MLutXmp.iptc.Location",                  &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.SpecialInstructions","MLutXmp.photoshop.Instructions",         &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.DateCreated",        "MLutXmp.photoshop.DateCreated",          &Converter::cnvNone, &Converter::cnvXmpValueToIptc }, // FIXME to IPTC Date and IPTC Time
        { mdIptc, "Iptc.Application2.DigitizationDate",   "MLutXmp.mlut_xmp.CreateDate",                 &Converter::cnvNone, &Converter::cnvXmpValueToIptc }, // FIXME to IPTC Date and IPTC Time
        { mdIptc, "Iptc.Application2.Byline",             "MLutXmp.dc.creator",                     &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.BylineTitle",        "MLutXmp.photoshop.AuthorsPosition",      &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.City",               "MLutXmp.photoshop.City",                 &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.ProvinceState",      "MLutXmp.photoshop.State",                &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.CountryCode",        "MLutXmp.iptc.CountryCode",               &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.CountryName",        "MLutXmp.photoshop.Country",              &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.TransmissionReference", "MLutXmp.photoshop.TransmissionReference", &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.Headline",            "MLutXmp.photoshop.Headline",            &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.Credit",             "MLutXmp.photoshop.Credit",               &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.Source",             "MLutXmp.photoshop.Source",               &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.Copyright",          "MLutXmp.dc.rights",                      &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.Caption",            "MLutXmp.dc.description",                 &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc },
        { mdIptc, "Iptc.Application2.Writer",             "MLutXmp.photoshop.CaptionWriter",        &Converter::cnvIptcValue, &Converter::cnvXmpValueToIptc }

    };

    Converter::Converter(ExifData& exifData, XmpData& xmpData)
        : erase_(false), overwrite_(true), exifData_(&exifData), iptcData_(0), xmpData_(&xmpData), iptcCharset_(0)
    {
    }

    Converter::Converter(IptcData& iptcData, XmpData& xmpData, const char *iptcCharset)
        : erase_(false), overwrite_(true), exifData_(0), iptcData_(&iptcData), xmpData_(&xmpData), iptcCharset_(iptcCharset)
    {
    }

    void Converter::cnvToXmp()
    {
        for (unsigned int i = 0; i < EXV_COUNTOF(conversion_); ++i) {
            const Conversion& c = conversion_[i];
            if (   (c.metadataId_ == mdExif && exifData_)
                || (c.metadataId_ == mdIptc && iptcData_)) {
                EXV_CALL_MEMBER_FN(*this, c.key1ToKey2_)(c.key1_, c.key2_);
            }
        }
    }

    void Converter::cnvFromXmp()
    {
        for (unsigned int i = 0; i < EXV_COUNTOF(conversion_); ++i) {
            const Conversion& c = conversion_[i];
            if (   (c.metadataId_ == mdExif && exifData_)
                || (c.metadataId_ == mdIptc && iptcData_)) {
                EXV_CALL_MEMBER_FN(*this, c.key2ToKey1_)(c.key2_, c.key1_);
            }
        }
    }

    void Converter::cnvNone(const char*, const char*)
    {
        return;
    }

    bool Converter::prepareExifTarget(const char* to, bool force)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(to));
        if (pos == exifData_->end()) return true;
        if (!overwrite_ && !force) return false;
        exifData_->erase(pos);
        return true;
    }

    bool Converter::prepareIptcTarget(const char* to, bool force)
    {
        Exiv2::IptcData::iterator pos = iptcData_->findKey(IptcKey(to));
        if (pos == iptcData_->end()) return true;
        if (!overwrite_ && !force) return false;
        while ((pos = iptcData_->findKey(IptcKey(to))) != iptcData_->end()) {
            iptcData_->erase(pos);
        }
        return true;
    }

    bool Converter::prepareXmpTarget(const char* to, bool force)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(to));
        if (pos == xmpData_->end()) return true;
        if (!overwrite_ && !force) return false;
        xmpData_->erase(pos);
        return true;
    }

    void Converter::cnvExifValue(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end()) return;
        std::string value = pos->toString();
        if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        if (!prepareXmpTarget(to)) return;
        (*xmpData_)[to] = value;
        if (erase_) exifData_->erase(pos);
    }

    void Converter::cnvExifComment(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end()) return;
        if (!prepareXmpTarget(to)) return;
        const CommentValue* cv = dynamic_cast<const CommentValue*>(&pos->value());
        if (cv == 0) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        // Todo: Convert to UTF-8 if necessary
        (*xmpData_)[to] = cv->comment();
        if (erase_) exifData_->erase(pos);
    }

    void Converter::cnvExifArray(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end()) return;
        if (!prepareXmpTarget(to)) return;
        for (int i = 0; i < pos->count(); ++i) {
            std::string value = pos->toString(i);
            if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }
            (*xmpData_)[to] = value;
        }
        if (erase_) exifData_->erase(pos);
    }

    void Converter::cnvExifDate(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end()) return;
        if (!prepareXmpTarget(to)) return;
        int year, month, day, hour, min, sec;
        std::string subsec;
        char buf[30];

        if (std::string(from) != "Exif.GPSInfo.GPSTimeStamp") {
            std::string value = pos->toString();
            if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }
            if (sscanf(value.c_str(), "%d:%d:%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec) != 6) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to
                            << ", unable to parse '" << value << "'\n";
#endif
                return;
            }
        }
        else { // "Exif.GPSInfo.GPSTimeStamp"

            bool ok = true;
            if (pos->count() != 3) ok = false;
            if (ok) {
                for (int i = 0; i < 3; ++i) {
                    if (pos->toRational(i).second == 0) {
                        ok = false;
                        break;
                    }
                }
            }
            if (!ok) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }

            double dhour = pos->toFloat(0);
            double dmin = pos->toFloat(1);
            // Hack: Need Value::toDouble
            Rational r = pos->toRational(2);
            double dsec = static_cast<double>(r.first)/r.second;

            if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }

            dsec = dhour * 3600.0 + dmin * 60.0 + dsec;

            hour = static_cast<int>(dsec / 3600.0);
            dsec -= hour * 3600;
            min = static_cast<int>(dsec / 60.0);
            dsec -= min * 60;
            sec = static_cast<int>(dsec);
            dsec -= sec;

            snprintf(buf, sizeof(buf), "%.9f", dsec);
            buf[sizeof(buf) - 1] = 0;
            buf[1] = '.'; // some locales use ','
            subsec = buf + 1;

            Exiv2::ExifData::iterator datePos = exifData_->findKey(ExifKey("Exif.GPSInfo.GPSDateStamp"));
            if (datePos == exifData_->end()) {
                datePos = exifData_->findKey(ExifKey("Exif.Photo.DateTimeOriginal"));
            }
            if (datePos == exifData_->end()) {
                datePos = exifData_->findKey(ExifKey("Exif.Photo.DateTimeDigitized"));
            }
            if (datePos == exifData_->end()) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }
            std::string value = datePos->toString();
            if (sscanf(value.c_str(), "%d:%d:%d", &year, &month, &day) != 3) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to
                            << ", unable to parse '" << value << "'\n";
#endif
                return;
            }
        }

        const char* subsecTag = 0;
        if (std::string(from) == "Exif.Image.DateTime") {
            subsecTag = "Exif.Photo.SubSecTime";
        }
        else if (std::string(from) == "Exif.Photo.DateTimeOriginal") {
            subsecTag = "Exif.Photo.SubSecTimeOriginal";
        }
        else if (std::string(from) == "Exif.Photo.DateTimeDigitized") {
            subsecTag = "Exif.Photo.SubSecTimeDigitized";
        }

        if (subsecTag) {
            ExifData::iterator subsec_pos = exifData_->findKey(ExifKey(subsecTag));
            if (   subsec_pos != exifData_->end()
                && subsec_pos->typeId() == asciiString) {
                std::string ss = subsec_pos->toString();
                if (!ss.empty()) {
                    bool ok = false;
                    stringTo<long>(ss, ok);
                    if (ok) subsec = std::string(".") + ss;
                }
            }
            if (erase_) exifData_->erase(subsec_pos);
        }

        if (subsec.size() > 10) subsec = subsec.substr(0, 10);
        snprintf(buf, sizeof(buf), "%4d-%02d-%02dT%02d:%02d:%02d%s",
                 year, month, day, hour, min, sec, subsec.c_str());
        buf[sizeof(buf) - 1] = 0;

        (*xmpData_)[to] = buf;
        if (erase_) exifData_->erase(pos);
    }

    void Converter::cnvExifVersion(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end()) return;
        if (!prepareXmpTarget(to)) return;
        std::ostringstream value;
        for (int i = 0; i < pos->count(); ++i) {
            value << static_cast<char>(pos->toLong(i));
        }
        (*xmpData_)[to] = value.str();
        if (erase_) exifData_->erase(pos);
    }

    void Converter::cnvExifGPSVersion(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end()) return;
        if (!prepareXmpTarget(to)) return;
        std::ostringstream value;
        for (int i = 0; i < pos->count(); ++i) {
            if (i > 0) value << '.';
            value << pos->toLong(i);
        }
        (*xmpData_)[to] = value.str();
        if (erase_) exifData_->erase(pos);
    }

    void Converter::cnvExifFlash(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end() || pos->count() == 0) return;
        if (!prepareXmpTarget(to)) return;
        int value = pos->toLong();
        if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }

        (*xmpData_)["MLutXmp.exif.Flash/exif:Fired"] = static_cast<bool>(value & 1);
        (*xmpData_)["MLutXmp.exif.Flash/exif:Return"] = (value >> 1) & 3;
        (*xmpData_)["MLutXmp.exif.Flash/exif:Mode"] = (value >> 3) & 3;
        (*xmpData_)["MLutXmp.exif.Flash/exif:Function"] = static_cast<bool>((value >> 5) & 1);
        (*xmpData_)["MLutXmp.exif.Flash/exif:RedEyeMode"] = static_cast<bool>((value >> 6) & 1);

        if (erase_) exifData_->erase(pos);
    }

    void Converter::cnvExifGPSCoord(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end()) return;
        if (!prepareXmpTarget(to)) return;
        if (pos->count() != 3) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        Exiv2::ExifData::iterator refPos = exifData_->findKey(ExifKey(std::string(from) + "Ref"));
        if (refPos == exifData_->end()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        double deg[3];
        for (int i = 0; i < 3; ++i) {
            const int32_t z = pos->toRational(i).first;
            const int32_t d = pos->toRational(i).second;
            if (d == 0) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }
            // Hack: Need Value::toDouble
            deg[i] = static_cast<double>(z)/d;
        }
        double min = deg[0] * 60.0 + deg[1] + deg[2] / 60.0;
        int ideg = static_cast<int>(min / 60.0);
        min -= ideg * 60;
        std::ostringstream oss;
        oss << ideg << ","
            << std::fixed << std::setprecision(7) << min
            << refPos->toString().c_str()[0];
        (*xmpData_)[to] = oss.str();

        if (erase_) exifData_->erase(pos);
        if (erase_) exifData_->erase(refPos);
    }

    void Converter::cnvXmpValue(const char* from, const char* to)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        if (!prepareExifTarget(to)) return;
        std::string value;
        if (!getTextValue(value, pos)) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        // Todo: Escape non-ASCII characters in XMP text values
        ExifKey key(to);
        Exifdatum ed(key);
        if (0 == ed.setValue(value)) {
            exifData_->add(ed);
        }
        if (erase_) xmpData_->erase(pos);
    }

    void Converter::cnvXmpComment(const char* from, const char* to)
    {
        if (!prepareExifTarget(to)) return;
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        std::string value;
        if (!getTextValue(value, pos)) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        // Assumes the XMP value is encoded in UTF-8, as it should be
        (*exifData_)[to] = "charset=Unicode " + value;
        if (erase_) xmpData_->erase(pos);
    }

    void Converter::cnvXmpArray(const char* from, const char* to)
    {
        if (!prepareExifTarget(to)) return;
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        std::ostringstream array;
        for (int i = 0; i < pos->count(); ++i) {
            std::string value = pos->toString(i);
            if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }
            array << value;
            if (i != pos->count() - 1) array << " ";
        }
        (*exifData_)[to] = array.str();
        if (erase_) xmpData_->erase(pos);
    }

    void Converter::cnvXmpDate(const char* from, const char* to)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        if (!prepareExifTarget(to)) return;
#ifdef EXV_HAVE_XMP_TOOLKIT
        std::string value = pos->toString();
        if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        XMP_DateTime datetime;
        try {
            SXMPUtils::ConvertToDate(value, &datetime);
        }
#ifndef SUPPRESS_WARNINGS
        catch (const XMP_Error& e) {
            EXV_WARNING << "Failed to convert " << from << " to " << to << " (" << e.GetErrMsg() << ")\n";
            return;
        }
#else
        catch (const XMP_Error&) {
            return;
        }
#endif // SUPPRESS_WARNINGS
        char buf[30];
        if (std::string(to) != "Exif.GPSInfo.GPSTimeStamp") {

            SXMPUtils::ConvertToLocalTime(&datetime);

            snprintf(buf, sizeof(buf), "%4d:%02d:%02d %02d:%02d:%02d",
                     static_cast<int>(datetime.year),
                     static_cast<int>(datetime.month),
                     static_cast<int>(datetime.day),
                     static_cast<int>(datetime.hour),
                     static_cast<int>(datetime.minute),
                     static_cast<int>(datetime.second));
            buf[sizeof(buf) - 1] = 0;
            (*exifData_)[to] = buf;

            if (datetime.nanoSecond) {
                const char* subsecTag = 0;
                if (std::string(to) == "Exif.Image.DateTime") {
                    subsecTag = "Exif.Photo.SubSecTime";
                }
                else if (std::string(to) == "Exif.Photo.DateTimeOriginal") {
                    subsecTag = "Exif.Photo.SubSecTimeOriginal";
                }
                else if (std::string(to) == "Exif.Photo.DateTimeDigitized") {
                    subsecTag = "Exif.Photo.SubSecTimeDigitized";
                }
                if (subsecTag) {
                    prepareExifTarget(subsecTag, true);
                    (*exifData_)[subsecTag] = toString(datetime.nanoSecond);
                }
            }
        }
        else { // "Exif.GPSInfo.GPSTimeStamp"

            // Ignore the time zone, assuming the time is in UTC as it should be

            URational rhour(datetime.hour, 1);
            URational rmin(datetime.minute, 1);
            URational rsec(datetime.second, 1);
            if (datetime.nanoSecond != 0) {
                if (datetime.second != 0) {
                    // Add the seconds to rmin so that the ns fit into rsec
                    rmin.second = 60;
                    rmin.first *= 60;
                    rmin.first += datetime.second;
                }
                rsec.second = 1000000000;
                rsec.first = datetime.nanoSecond;
            }

            std::ostringstream array;
            array << rhour << " " << rmin << " " << rsec;
            (*exifData_)[to] = array.str();

            prepareExifTarget("Exif.GPSInfo.GPSDateStamp", true);
            snprintf(buf, sizeof(buf), "%4d:%02d:%02d",
                    static_cast<int>(datetime.year),
                    static_cast<int>(datetime.month),
                    static_cast<int>(datetime.day));
            buf[sizeof(buf) - 1] = 0;
            (*exifData_)["Exif.GPSInfo.GPSDateStamp"] = buf;
        }

        if (erase_) xmpData_->erase(pos);
#else
# ifndef SUPPRESS_WARNINGS
        EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
# endif
#endif // !EXV_HAVE_XMP_TOOLKIT
    }

    void Converter::cnvXmpVersion(const char* from, const char* to)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        if (!prepareExifTarget(to)) return;
        std::string value = pos->toString();
        if (!pos->value().ok() || value.length() < 4) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        std::ostringstream array;

        array << static_cast<int>(value[0]) << " "
              << static_cast<int>(value[1]) << " "
              << static_cast<int>(value[2]) << " "
              << static_cast<int>(value[3]);

        (*exifData_)[to] = array.str();
        if (erase_) xmpData_->erase(pos);
    }

    void Converter::cnvXmpGPSVersion(const char* from, const char* to)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        if (!prepareExifTarget(to)) return;
        std::string value = pos->toString();
        if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }

        for (unsigned i = 0; i < value.length(); ++i) {
            if (value[i] == '.') value[i] = ' ';
        }
        (*exifData_)[to] = value;
        if (erase_) xmpData_->erase(pos);

    }

    void Converter::cnvXmpFlash(const char* from, const char* to)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(std::string(from) + "/exif:Fired"));
        if (pos == xmpData_->end()) return;
        if (!prepareExifTarget(to)) return;
        unsigned short value = 0;

        if (pos != xmpData_->end() && pos->count() > 0) {
            int fired = pos->toLong();
            if (pos->value().ok())
                value |= fired & 1;
#ifndef SUPPRESS_WARNINGS
            else
                EXV_WARNING << "Failed to convert " << std::string(from) + "/exif:Fired" << " to " << to << "\n";
#endif
        }
        pos = xmpData_->findKey(XmpKey(std::string(from) + "/exif:Return"));
        if (pos != xmpData_->end() && pos->count() > 0) {
            int ret = pos->toLong();
            if (pos->value().ok())
                value |= (ret & 3) << 1;
#ifndef SUPPRESS_WARNINGS
            else
                EXV_WARNING << "Failed to convert " << std::string(from) + "/exif:Return" << " to " << to << "\n";
#endif
        }
        pos = xmpData_->findKey(XmpKey(std::string(from) + "/exif:Mode"));
        if (pos != xmpData_->end() && pos->count() > 0) {
            int mode = pos->toLong();
            if (pos->value().ok())
                value |= (mode & 3) << 3;
#ifndef SUPPRESS_WARNINGS
            else
                EXV_WARNING << "Failed to convert " << std::string(from) + "/exif:Mode" << " to " << to << "\n";
#endif
        }
        pos = xmpData_->findKey(XmpKey(std::string(from) + "/exif:Function"));
        if (pos != xmpData_->end() && pos->count() > 0) {
            int function = pos->toLong();
            if (pos->value().ok())
                value |= (function & 1) << 5;
#ifndef SUPPRESS_WARNINGS
            else
                EXV_WARNING << "Failed to convert " << std::string(from) + "/exif:Function" << " to " << to << "\n";
#endif
        }
        pos = xmpData_->findKey(XmpKey(std::string(from) + "/exif:RedEyeMode"));
        if (pos != xmpData_->end() && pos->count() > 0) {
            int red = pos->toLong();
            if (pos->value().ok())
                value |= (red & 1) << 6;
#ifndef SUPPRESS_WARNINGS
            else
                EXV_WARNING << "Failed to convert " << std::string(from) + "/exif:RedEyeMode" << " to " << to << "\n";
#endif
        }

        (*exifData_)[to] = value;
        if (erase_) xmpData_->erase(pos);
    }

    void Converter::cnvXmpGPSCoord(const char* from, const char* to)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        if (!prepareExifTarget(to)) return;
        std::string value = pos->toString();
        if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        if (value.empty()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << from << " is empty\n";
#endif
            return;
        }

        double deg = 0.0;
        double min = 0.0;
        double sec = 0.0;
        char ref  = value[value.length() - 1];
        char sep1 = '\0';
        char sep2 = '\0';

        value.erase(value.length() - 1);

        std::istringstream in(value);

        in >> deg >> sep1 >> min >> sep2;

        if (sep2 == ',') {
            in >> sec;
        }
        else {
            sec = (min - static_cast<int>(min)) * 60.0;
            min = static_cast<double>(static_cast<int>(min));
            sep2 = ',';
        }

        if (   in.bad() || !(ref == 'N' || ref == 'S' || ref == 'E' || ref == 'W')
            || sep1 != ',' || sep2 != ',' || !in.eof()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }

        Rational rdeg = floatToRationalCast(static_cast<float>(deg));
        Rational rmin = floatToRationalCast(static_cast<float>(min));
        Rational rsec = floatToRationalCast(static_cast<float>(sec));

        std::ostringstream array;
        array << rdeg << " " << rmin << " " << rsec;
        (*exifData_)[to] = array.str();

        prepareExifTarget((std::string(to) + "Ref").c_str(), true);
        char ref_str[2] = {ref, 0};
        (*exifData_)[std::string(to) + "Ref"] = ref_str;

        if (erase_) xmpData_->erase(pos);
    }

    void Converter::cnvIptcValue(const char* from, const char* to)
    {
        Exiv2::IptcData::iterator pos = iptcData_->findKey(IptcKey(from));
        if (pos == iptcData_->end()) return;
        if (!prepareXmpTarget(to)) return;
        while (pos != iptcData_->end()) {
            if (pos->key() == from) {
                std::string value = pos->toString();
                if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                    ++pos;
                    continue;
                }
                if (iptcCharset_) convertStringCharset(value, iptcCharset_, "UTF-8");
                (*xmpData_)[to] = value;
                if (erase_) {
                    pos = iptcData_->erase(pos);
                    continue;
                }
            }
            ++pos;
        }
    }

    void Converter::cnvXmpValueToIptc(const char* from, const char* to)
    {
        XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        if (!prepareIptcTarget(to)) return;

        if (pos->typeId() == langAlt || pos->typeId() == xmpText) {
            std::string value;
            if (!getTextValue(value, pos)) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }
            (*iptcData_)[to] = value;
            (*iptcData_)["Iptc.Envelope.CharacterSet"] = "\033%G"; // indicate UTF-8 encoding
            if (erase_) xmpData_->erase(pos);
            return;
        }

        int count = pos->count();
        bool added = false;
        for (int i = 0; i < count; ++i) {
            std::string value = pos->toString(i);
            if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                continue;
            }
            IptcKey key(to);
            Iptcdatum id(key);
            id.setValue(value);
            iptcData_->add(id);
            added = true;
        }
        if (added) (*iptcData_)["Iptc.Envelope.CharacterSet"] = "\033%G"; // indicate UTF-8 encoding
        if (erase_) xmpData_->erase(pos);
    }

#ifdef EXV_HAVE_XMP_TOOLKIT
    std::string Converter::computeExifDigest(bool tiff)
    {
        std::ostringstream res;
        MD5_CTX    context;
        unsigned char digest[16];

        MD5Init ( &context );
        for (unsigned int i = 0; i < EXV_COUNTOF(conversion_); ++i) {
            const Conversion& c = conversion_[i];
            if (c.metadataId_ == mdExif) {
                Exiv2::ExifKey key(c.key1_);
                if (tiff && key.groupName() != "Image") continue;
                if (!tiff && key.groupName() == "Image") continue;

                if (!res.str().empty()) res << ',';
                res << key.tag();
                Exiv2::ExifData::iterator pos = exifData_->findKey(key);
                if (pos == exifData_->end()) continue;
                DataBuf data(pos->size());
                pos->copy(data.pData_, littleEndian /* FIXME ? */);
                MD5Update ( &context, data.pData_, data.size_);
            }
        }
        MD5Final(digest, &context);
        res << ';';
        res << std::setw(2) << std::setfill('0') << std::hex << std::uppercase;
        for (int i = 0; i < 16; ++i) {
            res << static_cast<int>(digest[i]);
        }
        return res.str();
    }
#else
    std::string Converter::computeExifDigest(bool)
    {
        return std::string("");
    }
#endif


    void Converter::writeExifDigest()
    {
#ifdef EXV_HAVE_XMP_TOOLKIT
        (*xmpData_)["MLutXmp.tiff.NativeDigest"] = computeExifDigest(true);
        (*xmpData_)["MLutXmp.exif.NativeDigest"] = computeExifDigest(false);
#endif
    }

    void Converter::syncExifWithXmp()
    {
        Exiv2::XmpData::iterator td = xmpData_->findKey(XmpKey("MLutXmp.tiff.NativeDigest"));
        Exiv2::XmpData::iterator ed = xmpData_->findKey(XmpKey("MLutXmp.exif.NativeDigest"));
        if (td != xmpData_->end() && ed != xmpData_->end()) {
            if (td->value().toString() == computeExifDigest(true) &&
                ed->value().toString() == computeExifDigest(false)) {
                // We have both digests and the values match
                // XMP is up-to-date, we should update Exif
                setOverwrite(true);
                setErase(false);

                cnvFromXmp();
                writeExifDigest();
                return;
            }
            else {
                // We have both digests and the values do not match
                // Exif was modified after XMP, we should update XMP
                setOverwrite(true);
                setErase(false);

                cnvToXmp();
                writeExifDigest();
                return;
            }
        }
        else {
            // We don't have both digests, it is probably the first conversion to XMP
            setOverwrite(false); // to be safe
            setErase(false);

            cnvToXmp();
            writeExifDigest();
            return;
        }
    }

    // *************************************************************************
    // free functions
    void copyExifToXmp(const ExifData& exifData, XmpData& xmpData)
    {
        Converter converter(const_cast<ExifData&>(exifData), xmpData);
        converter.cnvToXmp();
    }

    void moveExifToXmp(ExifData& exifData, XmpData& xmpData)
    {
        Converter converter(exifData, xmpData);
        converter.setErase();
        converter.cnvToXmp();
    }

    void copyXmpToExif(const XmpData& xmpData, ExifData& exifData)
    {
        Converter converter(exifData, const_cast<XmpData&>(xmpData));
        converter.cnvFromXmp();
    }

    void moveXmpToExif(XmpData& xmpData, ExifData& exifData)
    {
        Converter converter(exifData, xmpData);
        converter.setErase();
        converter.cnvFromXmp();
    }

    void syncExifWithXmp(ExifData& exifData, XmpData& xmpData)
    {
        Converter converter(exifData, xmpData);
        converter.syncExifWithXmp();
    }

    void copyIptcToXmp(const IptcData& iptcData, XmpData& xmpData, const char *iptcCharset)
    {
        if (!iptcCharset) iptcCharset = iptcData.detectCharset();
        if (!iptcCharset) iptcCharset = "ISO-8859-1";

        Converter converter(const_cast<IptcData&>(iptcData), xmpData, iptcCharset);
        converter.cnvToXmp();
    }

    void moveIptcToXmp(IptcData& iptcData, XmpData& xmpData, const char *iptcCharset)
    {
        if (!iptcCharset) iptcCharset = iptcData.detectCharset();
        if (!iptcCharset) iptcCharset = "ISO-8859-1";
        Converter converter(iptcData, xmpData, iptcCharset);
        converter.setErase();
        converter.cnvToXmp();
    }

    void copyXmpToIptc(const XmpData& xmpData, IptcData& iptcData)
    {
        Converter converter(iptcData, const_cast<XmpData&>(xmpData));
        converter.cnvFromXmp();
    }

    void moveXmpToIptc(XmpData& xmpData, IptcData& iptcData)
    {
        Converter converter(iptcData, xmpData);
        converter.setErase();
        converter.cnvFromXmp();
    }

    bool convertStringCharset(std::string &str, const char* from, const char* to)
    {
        if (0 == strcmp(from, to)) return true; // nothing to do
        bool ret = false;
#if defined EXV_HAVE_ICONV
        ret = convertStringCharsetIconv(str, from, to);
#elif defined WIN32 && !defined __CYGWIN__
        ret = convertStringCharsetWindows(str, from, to);
#else
# ifndef SUPPRESS_WARNINGS
        EXV_WARNING << "Charset conversion required but no character mapping functionality available.\n";
# endif
        UNUSED(str);
#endif
        return ret;
    }
}                                       // namespace Exiv2

// *****************************************************************************
// local definitions
namespace {

    using namespace Exiv2;

#if defined WIN32 && !defined __CYGWIN__
    bool swapBytes(std::string& str)
    {
        // Naive byte-swapping, I'm sure this can be done more efficiently
        if (str.size() & 1) {
#ifdef DEBUG
            EXV_DEBUG << "swapBytes: Size " << str.size() << " of input string is not even.\n";
#endif
            return false;
        }
        for (unsigned int i = 0; i < str.size() / 2; ++i) {
            char t = str[2 * i];
            str[2 * i] = str[2 * i + 1];
            str[2 * i + 1] = t;
        }
        return true;
    }

    bool mb2wc(UINT cp, std::string& str)
    {
        if (str.empty()) return true;
        int len = MultiByteToWideChar(cp, 0, str.c_str(), (int)str.size(), 0, 0);
        if (len == 0) {
#ifdef DEBUG
            EXV_DEBUG << "mb2wc: Failed to determine required size of output buffer.\n";
#endif
            return false;
        }
        std::vector<std::string::value_type> out;
        out.resize(len * 2);
        int ret = MultiByteToWideChar(cp, 0, str.c_str(), (int)str.size(), (LPWSTR)&out[0], len * 2);
        if (ret == 0) {
#ifdef DEBUG
            EXV_DEBUG << "mb2wc: Failed to convert the input string to a wide character string.\n";
#endif
            return false;
        }
        str.assign(out.begin(), out.end());
        return true;
    }

    bool wc2mb(UINT cp, std::string& str)
    {
        if (str.empty()) return true;
        if (str.size() & 1) {
#ifdef DEBUG
            EXV_DEBUG << "wc2mb: Size " << str.size() << " of input string is not even.\n";
#endif
            return false;
        }
        int len = WideCharToMultiByte(cp, 0, (LPCWSTR)str.data(), (int)str.size() / 2, 0, 0, 0, 0);
        if (len == 0) {
#ifdef DEBUG
            EXV_DEBUG << "wc2mb: Failed to determine required size of output buffer.\n";
#endif
            return false;
        }
        std::vector<std::string::value_type> out;
        out.resize(len);
        int ret = WideCharToMultiByte(cp, 0, (LPCWSTR)str.data(), (int)str.size() / 2, (LPSTR)&out[0], len, 0, 0);
        if (ret == 0) {
#ifdef DEBUG
            EXV_DEBUG << "wc2mb: Failed to convert the input string to a multi byte string.\n";
#endif
            return false;
        }
        str.assign(out.begin(), out.end());
        return true;
    }

    bool utf8ToUcs2be(std::string& str)
    {
        bool ret = mb2wc(CP_UTF8, str);
        if (ret) ret = swapBytes(str);
        return ret;
    }

    bool utf8ToUcs2le(std::string& str)
    {
        return mb2wc(CP_UTF8, str);
    }

    bool ucs2beToUtf8(std::string& str)
    {
        bool ret = swapBytes(str);
        if (ret) ret = wc2mb(CP_UTF8, str);
        return ret;
    }

    bool ucs2beToUcs2le(std::string& str)
    {
        return swapBytes(str);
    }

    bool ucs2leToUtf8(std::string& str)
    {
        return wc2mb(CP_UTF8, str);
    }

    bool ucs2leToUcs2be(std::string& str)
    {
        return swapBytes(str);
    }

    bool iso88591ToUtf8(std::string& str)
    {
        bool ret = mb2wc(28591, str);
        if (ret) ret = wc2mb(CP_UTF8, str);
        return ret;
    }

    bool asciiToUtf8(std::string& /*str*/)
    {
        // nothing to do
        return true;
    }

    typedef bool (*ConvFct)(std::string& str);

    struct ConvFctList {
        bool operator==(const std::pair<const char*, const char*> &fromTo) const
            { return 0 == strcmp(from_, fromTo.first) && 0 == strcmp(to_, fromTo.second); }
        const char* from_;
        const char* to_;
        ConvFct convFct_;
    };

    const ConvFctList convFctList[] = {
        { "UTF-8",      "UCS-2BE", utf8ToUcs2be   },
        { "UTF-8",      "UCS-2LE", utf8ToUcs2le   },
        { "UCS-2BE",    "UTF-8",   ucs2beToUtf8   },
        { "UCS-2BE",    "UCS-2LE", ucs2beToUcs2le },
        { "UCS-2LE",    "UTF-8",   ucs2leToUtf8   },
        { "UCS-2LE",    "UCS-2BE", ucs2leToUcs2be },
        { "ISO-8859-1", "UTF-8",   iso88591ToUtf8 },
        { "ASCII",      "UTF-8",   asciiToUtf8    }
        // Update the convertStringCharset() documentation if you add more here!
    };

    bool convertStringCharsetWindows(std::string& str, const char* from, const char* to)
    {
        bool ret = false;
        const ConvFctList* p = find(convFctList, std::make_pair(from, to));
        std::string tmpstr = str;
        if (p) ret = p->convFct_(tmpstr);
#ifndef SUPPRESS_WARNINGS
        else {
            EXV_WARNING << "No Windows function to map character string from " << from << " to " << to << " available.\n";
        }
#endif
        if (ret) str = tmpstr;
        return ret;
    }

#endif // defined WIN32 && !defined __CYGWIN__
#if defined EXV_HAVE_ICONV
    bool convertStringCharsetIconv(std::string& str, const char* from, const char* to)
    {
        if (0 == strcmp(from, to)) return true; // nothing to do

        bool ret = true;
        iconv_t cd;
        cd = iconv_open(to, from);
        if (cd == (iconv_t)(-1)) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "iconv_open: " << strError() << "\n";
#endif
            return false;
        }
        std::string outstr;
        EXV_ICONV_CONST char* inptr = const_cast<char*>(str.c_str());
        size_t inbytesleft = str.length();
        while (inbytesleft) {
            char outbuf[256];
            char* outptr = outbuf;
            size_t outbytesleft = sizeof(outbuf);
            size_t rc = iconv(cd,
                              &inptr,
                              &inbytesleft,
                              &outptr,
                              &outbytesleft);
            const size_t outbytesProduced = sizeof(outbuf) - outbytesleft;
            if (rc == size_t(-1) && errno != E2BIG) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "iconv: " << strError()
                            << " inbytesleft = " << inbytesleft << "\n";
#endif
                ret = false;
                break;
            }
            outstr.append(std::string(outbuf, outbytesProduced));
        }
        if (cd != (iconv_t)(-1)) {
            iconv_close(cd);
        }

        if (ret) str = outstr;
        return ret;
    }

#endif // EXV_HAVE_ICONV
    bool getTextValue(std::string& value, const XmpData::iterator& pos)
    {
        if (pos->typeId() == langAlt) {
            // get the default language entry without x-default qualifier
            value = pos->toString(0);
            if (!pos->value().ok() && pos->count() == 1) {
                // If there is no default but exactly one entry, take that
                // without the qualifier
                value = pos->toString();
                if (   pos->value().ok()
                    && value.length() > 5 && value.substr(0, 5) == "lang=") {
                    const std::string::size_type first_space_pos = value.find_first_of(' ');
                    if (first_space_pos != std::string::npos) {
                        value = value.substr(first_space_pos + 1);
                    }
                    else {
                        value.clear();
                    }
                }
            }
        }
        else {
            value = pos->toString();
        }
        return pos->value().ok();
    }

}
