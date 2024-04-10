import FreeTypeInit from "./freetype.js"
const Freetype = await FreeTypeInit()
const ft = Freetype

const useFreeType = true,
    caches = {},
    whitelistedFonts = new Set()

function parseFontString(str, cfonts = {}) {
    let split = str.split(" ")
    let weight = "normal"
    let splIndex = 1
    let size = split[0]
    if (!size.endsWith("px")) {
        // Probably a weight then
        splIndex++
        weight = split[0]
        size = split[1]
    }
    split = split
        .slice(splIndex)
        .join(" ")
        .split(/, ?/)
        .map((i) => i.replace(/"|'/gi, ""))
    let defaultFont = split[0]
    let fallbacks = split.slice(1)
    let fonts = []
    for (const font of [defaultFont, ...fallbacks]) {
        if (cfonts[font]) fonts.push(cfonts[font])
    }
    return { size, defaultFont, fallbacks, fonts, weight }
}
/**
 * Update glyph and bitmap caches
 *
 * @param {string} str
 * @param {DrawCache} cache
 */
async function updateCache(
    str,
    [r, g, b, a],
    cache,
    flags = Freetype.FT_LOAD_RENDER
) {
    // Get char codes without bitmaps
    const codes = []
    for (const char of new Set(str)) {
        const point = char.codePointAt(0)
        if (!cache.has(char) && point !== undefined) {
            codes.push(point)
        }
    }

    // Populate missing bitmaps
    const newGlyphs = Freetype.LoadGlyphs(codes, flags)
    for (const [code, glyph] of newGlyphs) {
        const char = String.fromCodePoint(code)
        if (glyph.bitmap.imagedata) {
            for (let x = 0; x < glyph.bitmap.imagedata.width; ++x) {
                for (let y = 0; y < glyph.bitmap.imagedata.height; ++y) {
                    // console.log(glyph.bitmap.width)
                    let base = (x + y * glyph.bitmap.width) * 4
                    const R = base,
                        G = base + 1,
                        B = base + 2,
                        A = base + 3
                    glyph.bitmap.imagedata.data[R] = (r * a) / 0xff
                    glyph.bitmap.imagedata.data[G] = (g * a) / 0xff
                    glyph.bitmap.imagedata.data[B] = (b * a) / 0xff
                    glyph.bitmap.imagedata.data[A] =
                        (glyph.bitmap.imagedata.data[A] * a) / 0xff
                    // glyph.bitmap.imagedata.data[R] = pR + ((nR - pR) * nA) / 255
                    // glyph.bitmap.imagedata.data[G] = pG + ((nG - pG) * nA) / 255
                    // glyph.bitmap.imagedata.data[B] = pB + ((nB - pB) * nA) / 255
                    // glyph.bitmap.imagedata.data[A] = Math.min(255, pA + nA)
                    // console.log(
                    //     `(${row}, ${pitch}) -> rgba(${pR}, ${pG}, ${pB}, ${pA})`
                    // )
                    // console.log(x, y, base)
                }
            }
        }
        let bitmap = glyph.bitmap.imagedata
            ? await createImageBitmap(glyph.bitmap.imagedata)
            : null
        cache.set(char, {
            glyph,
            bitmap,
        })
    }

    // TODO: Is awaiting with Promise.all faster? Is GPU uploading parallelizable?
}

function getFontSizeCache(font, weight, size, color) {
    let strkey = font + weight + size + color
    if (!caches[strkey]) caches[strkey] = { c: new Map(), strs: new Set() }
    return caches[strkey]
}

/**
 * @param {CanvasRenderingContext2D} ctx
 * @param {string} str
 * @param {number} offsetx
 * @param {number} offsety
 * @param {DrawCache} cache
 */
function write(ctx, str, col, offsetx, offsety, { c, strs }) {
    // offsety += Math.floor(7 / 2)
    offsetx = Math.ceil(offsetx)
    offsety = Math.ceil(offsety)
    let precached = strs.has(str)
    const fin = () => {
        let prev = null
        for (const char of str) {
            const { glyph, bitmap } = c.get(char) || {}
            if (glyph) {
                // Kerning
                if (prev) {
                    const kerning = Freetype.GetKerning(
                        prev.glyph_index,
                        glyph.glyph_index,
                        0
                    )
                    offsetx += kerning.x >> 6
                }
                if (bitmap) {
                    ctx.drawImage(
                        bitmap,
                        offsetx + glyph.bitmap_left,
                        offsety - glyph.bitmap_top
                    )
                }

                offsetx += glyph.advance.x >> 6
                prev = glyph
            }
            if (!precached) strs.add(str)
        }
    }
    if (precached) {
        fin()
    } else {
        updateCache(str, col, c, Freetype.FT_LOAD_RENDER).then(fin)
    }
}

/**
 * Create from URL
 *
 * @param {*} url
 * @returns {Promise<import("../dist/freetype.js").FT_FaceRec[]>}
 */
export async function createFontFromUrl(url) {
    const font = await fetch(url)
    const buffer = await font.arrayBuffer()
    const face = ft.LoadFontFromBytes(new Uint8Array(buffer))
    console.log(face)
    face.forEach((i) => whitelistedFonts.add(i.family_name))
    return face
}

/**
 * @param {CanvasRenderingContext2D} ctx
 */
export function hookCtx(ctx) {
    ;((fillText, measureText) => {
        const convT = {
            normal: "Regular",
            bold: "Bold",
        }
        // swaps properties in context and returns a function that restores
        // them to the originals
        const swap = (ctx, obj) => {
            let old = {}
            for (const prop in obj) {
                old[prop] = ctx[prop]
                ctx[prop] = obj[prop]
            }
            return () => {
                for (const prop in old) {
                    ctx[prop] = old[prop]
                }
            }
        }
        const setColor = (txt_) => {
            let txt = txt_
            if (txt.startsWith("#")) {
                if (txt.length == 7) {
                    // No alpha channel
                    txt = txt + "ff" // 255
                }
                let vals = txt
                    .substr(1)
                    .match(/.{2}/gi)
                    .map((i) => parseInt(i, 16))
                return vals
            } else if (txt.startsWith("rgba")) {
                let spl = txt
                    .substr("rgba(".length)
                    .split(", ")
                    .map((i) => parseFloat(i))
                spl[3] = ~~(spl[3] * 0xff)
                return spl
            } else {
                console.log(txt)
                throw "fuck off"
            }
        }
        const transformVec = (text, x, y) => {
            // ctx.measureText uses textBaseline and textAlign to measure ):
            let restore = swap(ctx, {
                textBaseline: "middle",
                textAlign: "start",
            })
            let tm = ctx.measureText(text)
            restore()
            switch (ctx.textBaseline) {
                case "alphabetic":
                    break
                case "hanging":
                case "top":
                    y = y + tm.actualBoundingBoxAscent
                    break
                case "ideographic":
                case "bottom":
                    tm = ctx.measureText(text)
                    y = y - tm.fontBoundingBoxDescent
                    break
                case "middle":
                    y = y + tm.fontBoundingBoxAscent / 2
                    break
            }
            switch (ctx.textAlign) {
                case "start":
                case "left":
                    break
                case "center":
                    x = x - tm.width / 2
                    break
                case "right":
                case "end":
                    x = x - tm.width
                    break
            }
            return [~~x, ~~y]
        }

        /**
         * @param {string} text
         * @param {number} x
         * @param {number} y
         * @param {number?} maxWidth
         */
        ctx.fillText = function (text, x, y, maxWidth) {
            if (useFreeType) {
                const font = parseFontString(this.font)
                if (!whitelistedFonts.has(font.defaultFont)) {
                    console.log(text, this.font, "def =", font.defaultFont)
                    throw "Font is non existent"
                    return
                }
                let fontSize = parseInt(font.size) // strips the px off
                const ftFont = ft.SetFont(font.defaultFont, convT[font.weight])

                let px = fontSize * 1,
                    py = fontSize * 1
                // px += 1
                const size = ft.SetPixelSize(px, py)
                const cmap = ft.SetCharmap(ft.FT_ENCODING_UNICODE)
                const cache = getFontSizeCache(
                    font.defaultFont,
                    font.weight,
                    fontSize,
                    this.fillStyle
                )
                const lineHeight = size.height >> 6
                let [tX, tY] = transformVec(text, x, y)

                write(
                    this,
                    text,
                    setColor(this.fillStyle),
                    tX,
                    tY,
                    cache,
                    lineHeight
                )
            } else {
                fillText(text, x, y, maxWidth)
            }
        }
    })(ctx.fillText.bind(ctx), ctx.measureText.bind(ctx))
}

export { ft }
