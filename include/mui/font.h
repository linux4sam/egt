/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_FONT_H
#define MUI_FONT_H

/**
 * @file
 * @brief Working with fonts.
 */

#include <string>

namespace mui
{

    /**
     * Manages a single font and properties of a font.
     *
     * A Font consists of a type face, a size, and a weight. The font face must
     * be installed on the system in order to use it.  Usually, if the specified
     * fonr face cannot be found on the system, a similar font face will be
     * selected.
     */
    class Font
    {
    public:

        /**
         * Font weights.
         */
        enum
        {
            WEIGHT_NORMAL = 0,
            WEIGHT_BOLD = 1,
        };


        /**
         * Font slants.
         */
        enum
        {
            SLANT_NORMAL = 0,
            SLANT_ITALIC = 1,
            SLANT_OBLIQUE = 2,
        };

        //@{
        /** @brief Pre-defined font faces. */
        static const std::string FACE_ARIAL;
        static const std::string FACE_TIMES;
        static const std::string FACE_COURIER;
        //@}

        /**
         * Set the global default Font.
         *
         * Call this before allocating any Font directly or indirectly (i.e.
         * with a Widget) to set the default font.
         */
        static void set_default_font(const Font& font);

        /**
         * Create a font based on the global default font.
         */
        Font();

        /**
         * Create a font based on the supplied parameters.
         *
         * @param[in] face The face name of the font.
         * @param[in] size The size of the font.
         * @param[in] weight The weight of the font.
         * @param[in] slant The slant of the font.
         */
        explicit Font(const std::string& face, int size, int weight, int slant);

        /**
         * Create a font based on the global default font, but with the
         * specified size.
         *
         * @param[in] size The size of the font.
         */
        explicit Font(int size);

        /**
         * Create a font based on the global default font, but with the
         * specified size and weight.
         *
         * @param[in] size The size of the font.
         * @param[in] weight The weight of the font.
         */
        explicit Font(int size, int weight);

        /**
         * Get the face name the font.
         */
        const std::string& face() const { return m_face; }

        /**
         * Get the size of the font.
         */
        virtual int size() const { return m_size; }

        /**
         * Set the size of the font.
         */
        virtual void size(int s) { m_size = s; }

        /**
         * Get the weight of the font.
         */
        virtual int weight() const { return m_weight; }

        /**
         * Get the slant of the font.
         */
        virtual int slant() const { return m_slant; }

        virtual ~Font()
        {}

    protected:

        std::string m_face;
        int m_size;
        int m_weight;
        int m_slant;
    };

}

#endif
