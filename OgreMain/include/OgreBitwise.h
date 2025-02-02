/*
-----------------------------------------------------------------------------
This source file is part of OGRE-Next
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef _Bitwise_H__
#define _Bitwise_H__

#include "OgrePrerequisites.h"

#include "OgreMath.h"

#ifndef __has_builtin
// Compatibility with non-clang compilers
#    define __has_builtin( x ) 0
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_FREEBSD
/// Undefine in <sys/endian.h> defined bswap macros for FreeBSD
#    undef bswap16
#    undef bswap32
#    undef bswap64
#endif

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#    include <intrin.h>
#    if OGRE_ARCH_TYPE == OGRE_ARCHITECTURE_32
#        pragma intrinsic( _BitScanForward )
#        pragma intrinsic( _BitScanReverse )
#    else
#        pragma intrinsic( _BitScanForward64 )
#        pragma intrinsic( _BitScanReverse64 )
#    endif
#endif

namespace Ogre
{
    /** \addtogroup Core
     *  @{
     */
    /** \addtogroup Math
     *  @{
     */

    /** Class for manipulating bit patterns.
     */
    class Bitwise
    {
    public:
        /** Returns value with reversed bytes order.
         */
        static FORCEINLINE uint16 bswap16( uint16 arg )
        {
#if OGRE_COMPILER == OGRE_COMPILER_MSVC && OGRE_COMP_VER >= 1310
            return _byteswap_ushort( arg );
#elif( OGRE_COMPILER == OGRE_COMPILER_CLANG && __has_builtin( __builtin_bswap16 ) ) || \
    ( OGRE_COMPILER == OGRE_COMPILER_GNUC && OGRE_COMP_VER >= 480 )
            return __builtin_bswap16( arg );
#else
            return ( ( arg << 8 ) & 0xFF00 ) | ( ( arg >> 8 ) & 0x00FF );
#endif
        }
        /** Returns value with reversed bytes order.
         */
        static FORCEINLINE uint32 bswap32( uint32 arg )
        {
#if OGRE_COMPILER == OGRE_COMPILER_MSVC && OGRE_COMP_VER >= 1310
            return _byteswap_ulong( arg );
#elif( OGRE_COMPILER == OGRE_COMPILER_CLANG && __has_builtin( __builtin_bswap32 ) ) || \
    ( OGRE_COMPILER == OGRE_COMPILER_GNUC && OGRE_COMP_VER >= 430 )
            return __builtin_bswap32( arg );
#else
            return ( ( arg & 0x000000FF ) << 24 ) | ( ( arg & 0x0000FF00 ) << 8 ) |
                   ( ( arg >> 8 ) & 0x0000FF00 ) | ( ( arg >> 24 ) & 0x000000FF );
#endif
        }
        /** Returns value with reversed bytes order.
         */
        static FORCEINLINE uint64 bswap64( uint64 arg )
        {
#if OGRE_COMPILER == OGRE_COMPILER_MSVC && OGRE_COMP_VER >= 1310
            return _byteswap_uint64( arg );
#elif( OGRE_COMPILER == OGRE_COMPILER_CLANG && __has_builtin( __builtin_bswap64 ) ) || \
    ( OGRE_COMPILER == OGRE_COMPILER_GNUC && OGRE_COMP_VER >= 430 )
            return __builtin_bswap64( arg );
#else
            union
            {
                uint64 sv;
                uint32 ul[2];
            } tmp, result;
            tmp.sv = arg;
            result.ul[0] = bswap32( tmp.ul[1] );
            result.ul[1] = bswap32( tmp.ul[0] );
            return result.sv;
#endif
        }

        /** Reverses byte order of buffer. Use bswap16/32/64 instead if possible.
         */
        static inline void bswapBuffer( void *pData, size_t size )
        {
            char swapByte;
            for( char *p0 = (char *)pData, *p1 = p0 + size - 1; p0 < p1; ++p0, --p1 )
            {
                swapByte = *p0;
                *p0 = *p1;
                *p1 = swapByte;
            }
        }
        /** Reverses byte order of chunks in buffer, where 'size' is size of one chunk.
         */
        static inline void bswapChunks( void *pData, size_t size, size_t count )
        {
            for( size_t c = 0; c < count; ++c )
            {
                char swapByte;
                for( char *p0 = (char *)pData + c * size, *p1 = p0 + size - 1; p0 < p1; ++p0, --p1 )
                {
                    swapByte = *p0;
                    *p0 = *p1;
                    *p1 = swapByte;
                }
            }
        }

        /** Returns the most significant bit set in a value.
         */
        static FORCEINLINE unsigned int mostSignificantBitSet( unsigned int value )
        {
            unsigned int result = 0;
            while( value != 0 )
            {
                ++result;
                value >>= 1;
            }
            return result - 1;
        }
        /** Returns the closest power-of-two number greater or equal to value.
            @note 0 and 1 are powers of two, so
                firstPO2From(0)==0 and firstPO2From(1)==1.
        */
        static FORCEINLINE uint32 firstPO2From( uint32 n )
        {
            --n;
            n |= n >> 16;
            n |= n >> 8;
            n |= n >> 4;
            n |= n >> 2;
            n |= n >> 1;
            ++n;
            return n;
        }
        /** Determines whether the number is power-of-two or not.
            @note 0 and 1 are tread as power of two.
        */
        template <typename T>
        static FORCEINLINE bool isPO2( T n )
        {
            return ( n & ( n - 1 ) ) == 0;
        }
        /** Returns the number of bits a pattern must be shifted right by to
            remove right-hand zeros.
        */
        template <typename T>
        static FORCEINLINE unsigned int getBitShift( T mask )
        {
            if( mask == 0 )
                return 0;

            unsigned int result = 0;
            while( ( mask & 1 ) == 0 )
            {
                ++result;
                mask >>= 1;
            }
            return result;
        }

        /** Takes a value with a given src bit mask, and produces another
            value with a desired bit mask.
            @remarks
                This routine is useful for colour conversion.
        */
        template <typename SrcT, typename DestT>
        static inline DestT convertBitPattern( SrcT srcValue, SrcT srcBitMask, DestT destBitMask )
        {
            // Mask off irrelevant source value bits (if any)
            srcValue = srcValue & srcBitMask;

            // Shift source down to bottom of DWORD
            const unsigned int srcBitShift = getBitShift( srcBitMask );
            srcValue >>= srcBitShift;

            // Get max value possible in source from srcMask
            const SrcT srcMax = srcBitMask >> srcBitShift;

            // Get max available in dest
            const unsigned int destBitShift = getBitShift( destBitMask );
            const DestT        destMax = destBitMask >> destBitShift;

            // Scale source value into destination, and shift back
            DestT destValue = ( srcValue * destMax ) / srcMax;
            return ( destValue << destBitShift );
        }

        /**
         * Convert N bit colour channel value to P bits. It fills P bits with the
         * bit pattern repeated. (this is /((1<<n)-1) in fixed point)
         */
        static inline unsigned int fixedToFixed( uint32 value, unsigned int n, unsigned int p )
        {
            if( n > p )
            {
                // Less bits required than available; this is easy
                value >>= n - p;
            }
            else if( n < p )
            {
                // More bits required than are there, do the fill
                // Use old fashioned division, probably better than a loop
                if( value == 0 )
                    value = 0;
                else if( value == ( static_cast<unsigned int>( 1 ) << n ) - 1 )
                    value = ( 1u << p ) - 1u;
                else
                    value = value * ( 1u << p ) / ( ( 1u << n ) - 1u );
            }
            return value;
        }

        /**
         * Convert floating point colour channel value between 0.0 and 1.0 (otherwise clamped)
         * to integer of a certain number of bits. Works for any value of bits between 0 and 31.
         */
        static inline unsigned int floatToFixed( const float value, const unsigned int bits )
        {
            if( value <= 0.0f )
                return 0;
            else if( value >= 1.0f )
                return ( 1u << bits ) - 1u;
            else
                return (unsigned int)( value * static_cast<float>( 1u << bits ) );
        }

        /**
         * Fixed point to float
         */
        static inline float fixedToFloat( unsigned value, unsigned int bits )
        {
            return (float)value / (float)( ( 1 << bits ) - 1 );
        }

        /**
         * Write a n*8 bits integer value to memory in native endian.
         */
        static inline void intWrite( void *dest, const int n, const unsigned int value )
        {
            switch( n )
            {
            case 1:
                ( (uint8 *)dest )[0] = (uint8)value;
                break;
            case 2:
                ( (uint16 *)dest )[0] = (uint16)value;
                break;
            case 3:
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
                ( (uint8 *)dest )[0] = (uint8)( ( value >> 16 ) & 0xFF );
                ( (uint8 *)dest )[1] = (uint8)( ( value >> 8 ) & 0xFF );
                ( (uint8 *)dest )[2] = (uint8)( value & 0xFF );
#else
                ( (uint8 *)dest )[2] = (uint8)( ( value >> 16 ) & 0xFF );
                ( (uint8 *)dest )[1] = (uint8)( ( value >> 8 ) & 0xFF );
                ( (uint8 *)dest )[0] = (uint8)( value & 0xFF );
#endif
                break;
            case 4:
                ( (uint32 *)dest )[0] = (uint32)value;
                break;
            }
        }
        /**
         * Read a n*8 bits integer value to memory in native endian.
         */
        static inline unsigned int intRead( const void *src, int n )
        {
            switch( n )
            {
            case 1:
                return ( (const uint8 *)src )[0];
            case 2:
                return ( (const uint16 *)src )[0];
            case 3:
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
                return ( (uint32)( (const uint8 *)src )[0] << 16 ) |
                       ( (uint32)( (const uint8 *)src )[1] << 8 ) |
                       ( (uint32)( (const uint8 *)src )[2] );
#else
                return ( (uint32)( (const uint8 *)src )[0] ) |
                       ( (uint32)( (const uint8 *)src )[1] << 8 ) |
                       ( (uint32)( (const uint8 *)src )[2] << 16 );
#endif
            case 4:
                return ( (const uint32 *)src )[0];
            }
            return 0;  // ?
        }

        /** Convert a float32 to a float16 (NV_half_float)
            Courtesy of OpenEXR
        */
        static inline uint16 floatToHalf( float i )
        {
            union
            {
                float  f;
                uint32 i;
            } v;
            v.f = i;
            return floatToHalfI( v.i );
        }
        /** Converts float in uint32 format to a a half in uint16 format
         */
        static inline uint16 floatToHalfI( uint32 i )
        {
            const int s = ( i >> 16 ) & 0x00008000;
            const int e = static_cast<int>( ( i >> 23 ) & 0x000000ff ) - ( 127 - 15 );
            int       m = i & 0x007fffff;

            if( e <= 0 )
            {
                if( e < -10 )
                {
                    return 0;
                }
                m = ( m | 0x00800000 ) >> ( 1 - e );

                return static_cast<uint16>( s | ( m >> 13 ) );
            }
            else if( e == 0xff - ( 127 - 15 ) )
            {
                if( m == 0 )  // Inf
                {
                    return static_cast<uint16>( s | 0x7c00 );
                }
                else  // NAN
                {
                    m >>= 13;
                    return static_cast<uint16>( s | 0x7c00 | m | ( m == 0 ) );
                }
            }
            else
            {
                if( e > 30 )  // Overflow
                {
                    return static_cast<uint16>( s | 0x7c00 );
                }

                return static_cast<uint16>( s | ( e << 10 ) | ( m >> 13 ) );
            }
        }

        /**
         * Convert a float16 (NV_half_float) to a float32
         * Courtesy of OpenEXR
         */
        static inline float halfToFloat( uint16 y )
        {
            union
            {
                float  f;
                uint32 i;
            } v;
            v.i = halfToFloatI( y );
            return v.f;
        }
        /** Converts a half in uint16 format to a float
            in uint32 format
         */
        static inline uint32 halfToFloatI( uint16 y )
        {
            const uint32 s = ( y >> 15 ) & 0x00000001;
            uint32       e = ( y >> 10 ) & 0x0000001f;
            uint32       m = y & 0x000003ff;

            if( e == 0 )
            {
                if( m == 0 )  // Plus or minus zero
                {
                    return s << 31;
                }
                else  // Denormalized number -- renormalize it
                {
                    while( !( m & 0x00000400 ) )
                    {
                        m <<= 1;
                        e -= 1;
                    }

                    e += 1;
                    m &= ~0x00000400u;
                }
            }
            else if( e == 31 )
            {
                if( m == 0 )  // Inf
                {
                    return ( s << 31u ) | 0x7f800000u;
                }
                else  // NaN
                {
                    return ( s << 31u ) | 0x7f800000u | ( m << 13u );
                }
            }

            e = e + ( 127 - 15 );
            m = m << 13;

            return ( s << 31 ) | ( e << 23 ) | m;
        }

        static inline int16 floatToSnorm16( float v )
        {
            // According to D3D10 rules, the value "-1.0f" has two representations:
            //  0x1000 and 0x10001
            // This allows everyone to convert by just multiplying by 32767 instead
            // of multiplying the negative values by 32768 and 32767 for positive.
            return static_cast<int16>( Math::Clamp(
                v >= 0.0f ? ( v * 32767.0f + 0.5f ) : ( v * 32767.0f - 0.5f ), -32768.0f, 32767.0f ) );
        }

        static inline float snorm16ToFloat( int16 v )
        {
            // -32768 & -32767 both map to -1 according to D3D10 rules.
            return std::max( v / 32767.0f, -1.0f );
        }

        static inline int8 floatToSnorm8( float v )
        {
            // According to D3D10 rules, the value "-1.0f" has two representations:
            //  0x10 and 0x11
            // This allows everyone to convert by just multiplying by 127 instead
            // of multiplying the negative values by 128 and 127 for positive.
            return static_cast<int8>( Math::Clamp(
                v >= 0.0f ? ( v * 127.0f + 0.5f ) : ( v * 127.0f - 0.5f ), -128.0f, 127.0f ) );
        }

        static inline float snorm8ToFloat( int8 v )
        {
            // -128 & -127 both map to -1 according to D3D10 rules.
            return std::max( v / 127.0f, -1.0f );
        }

        static inline uint32 ctz32( uint32 value )
        {
            if( value == 0 )
                return 32u;

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
            unsigned long trailingZero = 0;
            _BitScanForward( &trailingZero, value );
            return trailingZero;
#else
            return static_cast<uint32>( __builtin_ctz( value ) );
#endif
        }

        static inline uint32 clz32( uint32 value )
        {
            if( value == 0 )
                return 32u;

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
            unsigned long lastBitSet = 0;
            _BitScanReverse( &lastBitSet, value );
            return 31u - lastBitSet;
#else
            return static_cast<uint32>( __builtin_clz( value ) );
#endif
        }

        static inline uint32 ctz64( uint64 value )
        {
            if( value == 0 )
                return 64u;

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
            unsigned long trailingZero = 0;
#    if OGRE_ARCH_TYPE == OGRE_ARCHITECTURE_32
            // Scan the low 32 bits.
            if( _BitScanForward( &trailingZero, static_cast<uint32>( value ) ) )
                return trailingZero;

            // Scan the high 32 bits.
            _BitScanForward( &trailingZero, static_cast<uint32>( value >> 32u ) );
            trailingZero += 32u;
#    else
            _BitScanForward64( &trailingZero, value );
#    endif
            return trailingZero;
#else
            return static_cast<uint32>( __builtin_ctzll( value ) );
#endif
        }

        static inline uint32 clz64( uint64 value )
        {
            if( value == 0 )
                return 64u;

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
            unsigned long lastBitSet = 0;
#    if OGRE_ARCH_TYPE == OGRE_ARCHITECTURE_32
            // Scan the high 32 bits.
            if( _BitScanReverse( &lastBitSet, static_cast<uint32>( value >> 32u ) ) )
                return 63u - ( lastBitSet + 32u );

            // Scan the low 32 bits.
            _BitScanReverse( &lastBitSet, static_cast<uint32>( value ) );
#    else
            _BitScanReverse64( &lastBitSet, value );
#    endif
            return 63u - lastBitSet;
#else
            return static_cast<uint32>( __builtin_clzll( value ) );
#endif
        }
    };
    /** @} */
    /** @} */

}  // namespace Ogre

/** Redefine in <sys/endian.h> defined bswap macros for FreeBSD
 */
#if OGRE_PLATFORM == OGRE_PLATFORM_FREEBSD
#    define bswap16( x ) __bswap16( x )
#    define bswap32( x ) __bswap32( x )
#    define bswap64( x ) __bswap64( x )
#endif

#endif
