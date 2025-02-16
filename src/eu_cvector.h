/******************************************************************************
 * This file is part of c-vector project
 * From https://https://github.com/eteran/c-vector/list.h
 * The MIT License (MIT)
 * Copyright (c) 2015 Evan Teran
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#ifndef CVECTOR_H_
#define CVECTOR_H_

#include <assert.h> /* for assert */
#include <stdlib.h> /* for malloc/realloc/free */
#include <string.h> /* for memcpy/memmove */

/* cvector heap implemented using C library malloc() */

/* in case C library malloc() needs extra protection,
 * allow these defines to be overridden.
 */
#ifndef cvector_clib_free
#define cvector_clib_free free
#endif
#ifndef cvector_clib_malloc
#define cvector_clib_malloc malloc
#endif
#ifndef cvector_clib_calloc
#define cvector_clib_calloc calloc
#endif
#ifndef cvector_clib_realloc
#define cvector_clib_realloc realloc
#endif

/**
 * @brief cvector_vector_type - The vector type used in this library
 */
#define cvector_vector_type(type) type *

/**
 * @brief cvector_capacity - gets the current capacity of the vector
 * @param vec - the vector
 * @return the capacity as a size_t
 */
#define cvector_capacity(vec) \
    ((vec) ? ((size_t *)(vec))[-1] : (size_t)0)

/**
 * @brief cvector_size - gets the current size of the vector
 * @param vec - the vector
 * @return the size as a size_t
 */
#define cvector_size(vec) \
    ((vec) ? ((size_t *)(vec))[-2] : (size_t)0)

/**
 * @brief cvector_empty - returns non-zero if the vector is empty
 * @param vec - the vector
 * @return non-zero if empty, zero if non-empty
 */
#define cvector_empty(vec) \
    (cvector_size(vec) == 0)

/**
 * @brief cvector_reserve - Requests that the vector capacity be at least enough
 * to contain n elements. If n is greater than the current vector capacity, the
 * function causes the container to reallocate its storage increasing its
 * capacity to n (or greater).
 * @param vec - the vector
 * @param n - Minimum capacity for the vector.
 * @return void
 */
#define cvector_reserve(vec, capacity)           \
    do {                                         \
        size_t cv_cap__ = cvector_capacity(vec); \
        if (cv_cap__ < (capacity)) {             \
            cvector_grow((vec), (capacity));     \
        }                                        \
    } while (0)

/**
 * @brief cvector_erase - removes the element at index i from the vector
 * @param vec - the vector
 * @param i - index of element to remove
 * @return void
 */
#define cvector_erase(vec, i)                                                                \
    do {                                                                                     \
        if ((vec)) {                                                                         \
            const size_t cv_sz__ = cvector_size(vec);                                        \
            if ((i) < cv_sz__) {                                                             \
                cvector_set_size((vec), cv_sz__ - 1);                                        \
                memmove((vec) + (i), (vec) + (i) + 1, sizeof(*(vec)) * (cv_sz__ - 1 - (i))); \
            }                                                                                \
        }                                                                                    \
    } while (0)

/**
 * @brief cvector_free - frees all memory associated with the vector
 * @param vec - the vector
 * @return void
 */
#define cvector_free(vec)                        \
    do {                                         \
        if ((vec)) {                             \
            size_t *p1 = &((size_t *)(vec))[-2]; \
            cvector_clib_free(p1);               \
        }                                        \
    } while (0)

/**
 * @brief cvector_freep - frees all memory and assign null to the pointer
 * @param pvec - the vector pointer
 * @return void
 */
#define cvector_freep(pvec)                                 \
    do {                                                    \
        if ((pvec && *pvec)) {                              \
            void *val__;                                    \
            memcpy(&val__, (pvec), sizeof(val__));          \
            memcpy((pvec), &(void *){NULL}, sizeof(val__)); \
            cvector_free(val__);                            \
        }                                                   \
    } while (0)

/**
 * @brief cvector_begin - returns an iterator to first element of the vector
 * @param vec - the vector
 * @return a pointer to the first element (or NULL)
 */
#define cvector_begin(vec) \
    (vec)

/**
 * @brief cvector_end - returns an iterator to one past the last element of the vector
 * @param vec - the vector
 * @return a pointer to one past the last element (or NULL)
 */
#define cvector_end(vec) \
    ((vec) ? &((vec)[cvector_size(vec)]) : NULL)

/* user request to use logarithmic growth algorithm */
#ifdef CVECTOR_LOGARITHMIC_GROWTH

/**
 * @brief cvector_compute_next_grow - returns an the computed size in next vector grow
 * size is increased by multiplication of 2
 * @param size - current size
 * @return size after next vector grow
 */
#define cvector_compute_next_grow(size) \
    ((size) ? ((size) << 1) : 1)

#else

/**
 * @brief cvector_compute_next_grow - returns an the computed size in next vector grow
 * size is increased by 1
 * @param size - current size
 * @return size after next vector grow
 */
#define cvector_compute_next_grow(size) \
    ((size) + 1)

#endif /* CVECTOR_LOGARITHMIC_GROWTH */

/**
 * @brief cvector_push_back - adds an element to the end of the vector
 * @param vec - the vector
 * @param value - the value to add
 * @return void
 */
#define cvector_push_back(vec, value)                                 \
    do {                                                              \
        size_t cv_cap__ = cvector_capacity(vec);                      \
        if (cv_cap__ <= cvector_size(vec)) {                          \
            cvector_grow((vec), cvector_compute_next_grow(cv_cap__)); \
        }                                                             \
        (vec)[cvector_size(vec)] = (value);                           \
        cvector_set_size((vec), cvector_size(vec) + 1);               \
    } while (0)

/**
 * @brief cvector_insert - insert element at position pos to the vector
 * @param vec - the vector
 * @param pos - position in the vector where the new elements are inserted.
 * @param val - value to be copied (or moved) to the inserted elements.
 * @return void
 */
#define cvector_insert(vec, pos, val)                                                                      \
    do {                                                                                                   \
        if (cvector_capacity(vec) <= cvector_size(vec) + 1) {                                              \
            cvector_grow((vec), cvector_compute_next_grow(cvector_capacity((vec))));                       \
        }                                                                                                  \
        if ((pos) < cvector_size(vec)) {                                                                   \
            memmove((vec) + (pos) + 1, (vec) + (pos), sizeof(*(vec)) * ((cvector_size(vec) + 1) - (pos))); \
        }                                                                                                  \
        (vec)[(pos)] = (val);                                                                              \
        cvector_set_size((vec), cvector_size(vec) + 1);                                                    \
    } while (0)

/**
 * @brief cvector_pop_back - removes the last element from the vector
 * @param vec - the vector
 * @return void
 */
#define cvector_pop_back(vec)                           \
    do {                                                \
        cvector_set_size((vec), cvector_size(vec) - 1); \
    } while (0)

/**
 * @brief cvector_clear - empty the array, but not free memory
 * @param vec - the vector
 * @return void
 */
#define cvector_clear(vec)                           \
    do {                                             \
        cvector_set_size((vec), 0);                  \
    } while (0)

/**
 * @brief cvector_copy - copy a vector
 * @param from - the original vector
 * @param to - destination to which the function copy to
 * @return void
 */
#define cvector_copy(from, to)                                          \
    do {                                                                \
        if ((from)) {                                                   \
            cvector_grow(to, cvector_size(from));                       \
            cvector_set_size(to, cvector_size(from));                   \
            memcpy((to), (from), cvector_size(from) * sizeof(*(from))); \
        }                                                               \
    } while (0)

/**
 * @brief cvector_set_capacity - For internal use, sets the capacity variable of the vector
 * @param vec - the vector
 * @param size - the new capacity to set
 * @return void
 */
#define cvector_set_capacity(vec, size)     \
    do {                                    \
        if ((vec)) {                        \
            ((size_t *)(vec))[-1] = (size); \
        }                                   \
    } while (0)

/**
 * @brief cvector_set_size - For internal use, sets the size variable of the vector
 * @param vec - the vector
 * @param size - the new capacity to set
 * @return void
 */
#define cvector_set_size(vec, size)         \
    do {                                    \
        if ((vec)) {                        \
            ((size_t *)(vec))[-2] = (size); \
        }                                   \
    } while (0)

/**
 * @brief cvector_grow - For internal use, ensures that the vector is at least <count> elements big
 * @param vec - the vector
 * @param count - the new capacity to set
 * @return void
 */
#define cvector_grow(vec, count)                                                \
    do {                                                                        \
        const size_t cv_sz__ = (count) * sizeof(*(vec)) + (sizeof(size_t) * 2); \
        if ((vec)) {                                                            \
            size_t *cv_p1__ = &((size_t *)(vec))[-2];                           \
            size_t *cv_p2__ = cvector_clib_realloc(cv_p1__, (cv_sz__));         \
            assert(cv_p2__);                                                    \
            (vec) = (void *)(&cv_p2__[2]);                                      \
            cvector_set_capacity((vec), (count));                               \
        } else {                                                                \
            size_t *cv_p__ = cvector_clib_malloc(cv_sz__);                      \
            assert(cv_p__);                                                     \
            (vec) = (void *)(&cv_p__[2]);                                       \
            cvector_set_capacity((vec), (count));                               \
            cvector_set_size((vec), 0);                                         \
        }                                                                       \
    } while (0)

/**
 * @brief cvector_for_each - call function func on each element of the vector
 * @param vec - the vector
 * @param func - function to be called on each element that takes each element as argument
 * @return void
 */
#define cvector_for_each(vec, func)                          \
    do {                                                     \
        if ((vec) && (func) != NULL) {                       \
            for (size_t i = 0; i < cvector_size(vec); ++i) { \
                func((vec)[i]);                              \
            }                                                \
        }                                                    \
    } while (0)
    
/**
 * @brief cvector_for_each - call function with param on each element of the vector
 * @param vec - the vector
 * @param func - function to be called on each element that takes each element as argument
 * @return void
 */
#define cvector_for_each_and_do(vec, func, param)            \
    do {                                                     \
        if ((vec) && (func) != NULL) {                       \
            for (size_t i = 0; i < cvector_size(vec); ++i) { \
                func(&(vec)[i], (param));                    \
            }                                                \
        }                                                    \
    } while (0)    

/**
 * @brief cvector_for_each_and_cmp - call function func on each element of the vector
 * @param vec - the vector
 * @param func - function to be called on each element that takes each element as argument
 * @param param - function param
 * @param it_ - the vector pointer
 * @return void
 */    
#define cvector_for_each_and_cmp(vec, func, param, it_)      \
    do {                                                     \
        if ((vec) && (func) && (it_)) {                      \
            size_t i_ = 0;                                   \
            size_t len_ = cvector_size(vec);                 \
            for (; i_ < len_; ++i_) {                        \
                if (!func(&(vec)[i_], param)) {              \
                    break;                                   \
                }                                            \
            }                                                \
            if (i_ >= 0 && i_ < len_) {                      \
                *it_ = &(vec)[i_];                           \
            }                                                \
        }                                                    \
    } while (0)

/**
 * @brief cvector_free_each_and_free - calls `free_func` on each element
 * contained in the vector and then destroys the vector itself
 * @param vec - the vector
 * @param free_func - function used to free each element in the vector with
 * one parameter which is the element to be freed)
 * @return void
 */
#define cvector_free_each_and_free(vec, free_func) \
    do {                                           \
        cvector_for_each((vec), (free_func));      \
        cvector_free(vec);                         \
    } while (0)

#endif /* CVECTOR_H_ */
