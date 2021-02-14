/*
 * kb_matrix.h
 *
 *  Created on: Feb 13, 2021
 *      Author: makso
 */

#ifndef INC_KB_MATRIX_H_
#define INC_KB_MATRIX_H_


#if (MATRIX_COLS <= 8)
typedef  uint8_t    matrix_row_t;
#elif (MATRIX_COLS <= 16)
typedef  uint16_t   matrix_row_t;
#elif (MATRIX_COLS <= 32)
typedef  uint32_t   matrix_row_t;
#else
#error "MATRIX_COLS: invalid value"
#endif

#define MATRIX_IS_ON(row, col)  (matrix_get_row(row) && (1<<col))


uint8_t matrix_rows(void);
/* number of matrix columns */
uint8_t matrix_cols(void);
/* Initialize matrix for scanning. should be called once. */
void matrix_init(void);
/* scan all key states on matrix */
void matrix_scan(uint8_t code);
/* whether a swtich is on */
uint8_t matrix_is_on(uint8_t row, uint8_t col);
/* matrix state on row */
matrix_row_t  matrix_get_row(uint8_t row);

#endif /* INC_KB_MATRIX_H_ */
