	int deg_lambda, el, deg_omega;
	int i, j, r, k;
	uint16_t q, tmp, num1, num2, den, discr_r;
	symbol_t lambda[PARITY_SYMBOL_COUNT + 1], s[PARITY_SYMBOL_COUNT + 1]; /* Err Locator poly
	                                       						 * and syndrome poly */
	symbol_t b[PARITY_SYMBOL_COUNT + 1], t[PARITY_SYMBOL_COUNT + 1], omega[PARITY_SYMBOL_COUNT + 1];
	symbol_t root[PARITY_SYMBOL_COUNT], reg[PARITY_SYMBOL_COUNT + 1], loc[PARITY_SYMBOL_COUNT];
	int syn_error, count, corrections;

	/* form the syndromes; i.e., evaluate data(x) at roots of g(x) */
	for (i = 0; i < PARITY_SYMBOL_COUNT; i++) {
		s[i] = symbol_get(data, 0);
	}

	for (j = 1; j < TOTAL_SYMBOL_COUNT; j++) {
		for (i = 0; i < PARITY_SYMBOL_COUNT; i++) {
			if (s[i] == 0) {
				s[i] = symbol_get(data, j);
			}
			else {
				s[i] = symbol_get(data, j) ^ symbol_get(alpha_to, modnn(symbol_get(index_of, s[i]) + (FCS + i) * PRIM));
			}
		}
	}

	/* Convert syndromes to index form, checking for nonzero condition */
	syn_error = 0;
	for (i = 0; i < PARITY_SYMBOL_COUNT; i++) {
		syn_error |= s[i];
		s[i] = symbol_get(index_of, s[i]);
	}

	if (!syn_error) {
		/* if syndrome is zero, data[] is a codeword and there are no
		 * errors to correct. So return data[] unmodified
		 */
		return 0;
	}

	for (i = 1; i < PARITY_SYMBOL_COUNT + 1; i++) {
		lambda[i] = 0;
	}
	lambda[0] = 1;

	for (i = 0; i < PARITY_SYMBOL_COUNT + 1; i++) {
		b[i] = symbol_get(index_of, lambda[i]);
	}

	/*
	 * Begin Berlekamp-Massey algorithm to determine error
	 * locator polynomial
	 */
	r = 0;
	el = 0;
	while (++r <= PARITY_SYMBOL_COUNT) { /* r is the step number */
		/* Compute discrepancy at the r-th step in poly-form */
		discr_r = 0;
		for (i = 0; i < r; i++) {
			if ((lambda[i] != 0) && (s[r - i - 1] != SYMBOL_MASK)) {
				discr_r ^= symbol_get(alpha_to, modnn(symbol_get(index_of, lambda[i]) + s[r - i - 1]));
			}
		}
		discr_r = symbol_get(index_of, discr_r); /* Index form */
		if (discr_r == SYMBOL_MASK) {
		/* Shift */
			for(j = PARITY_SYMBOL_COUNT; j > 0; j--) {
				b[j] = b[j-1];
			}
			b[0] = SYMBOL_MASK;
		}
		else {
			/* 7 lines below: T(x) <-- lambda(x) - discr_r*x*b(x) */
			t[0] = lambda[0];
			for (i = 0; i < PARITY_SYMBOL_COUNT; i++) {
				if (b[i] != SYMBOL_MASK) {
					t[i + 1] = lambda[i + 1] ^ symbol_get(alpha_to, modnn(discr_r + b[i]));
				}
				else {
					t[i + 1] = lambda[i + 1];
				}
			}
			if (2 * el <= r + 0 - 1) {
				el = r + 0 - el;
				/*
				 * 2 lines below: B(x) <-- inv(discr_r) *
				 * lambda(x)
				 */
				for (i = 0; i <= PARITY_SYMBOL_COUNT; i++) {
					b[i] = (lambda[i] == 0) ? SYMBOL_MASK : modnn(symbol_get(index_of, lambda[i]) - discr_r + TOTAL_SYMBOL_COUNT);
				}
			}
			else {
			/* Shift */
				for(j = PARITY_SYMBOL_COUNT; j > 0; j--) {
					b[j] = b[j-1];
				}
				b[0] = SYMBOL_MASK;
			}

			for(j=0; j<(PARITY_SYMBOL_COUNT + 1); j++) {
				lambda[j] = t[j];
			}
		}
	}

	/* Convert lambda to index form and compute deg(lambda(x)) */
	deg_lambda = 0;
	for (i = 0; i < PARITY_SYMBOL_COUNT + 1; i++) {
		lambda[i] = symbol_get(index_of, lambda[i]);
		if (lambda[i] != SYMBOL_MASK) {
			deg_lambda = i;
		}
	}

	/* Find roots of the error locator polynomial by Chien search */
	for (i = 1; i < (PARITY_SYMBOL_COUNT + 1); i++) {
		reg[i] = lambda[i];
	}

	count = 0;  /* Number of roots of lambda(x) */
	for (i = 1, k = IPRIM - 1; i <= TOTAL_SYMBOL_COUNT; i++, k = modnn(k + IPRIM)) {
		q = 1;  /* lambda[0] is always 0 */
		for (j = deg_lambda; j > 0; j--) {
			if (reg[j] != SYMBOL_MASK) {
				reg[j] = modnn(reg[j] + j);
				q ^= symbol_get(alpha_to, reg[j]);
			}
		}
		if (q != 0) {
			continue; /* Not a root */
		}
		/* store root (index-form) and error location number */
		root[count] = i;
		loc[count] = k;
		/* If we've already found max possible roots,
		 * abort the search to save time
		 */
		if (++count == deg_lambda) {
			break;
		}
	}

	if (deg_lambda != count) {
		/*
		 * deg(lambda) unequal to number of roots => uncorrectable
		 * error detected
		 */
		return -5;
	}

	/*
	 * Compute err evaluator poly omega(x) = s(x)*lambda(x) (modulo
	 * x**PARITY_SYMBOL_COUNT). in index form. Also find deg(omega).
	 */
	deg_omega = deg_lambda - 1;
	for (i = 0; i <= deg_omega; i++) {
		tmp = 0;
		for (j = i; j >= 0; j--) {
			if ((s[i - j] != SYMBOL_MASK) && (lambda[j] != SYMBOL_MASK)) {
				tmp ^= symbol_get(alpha_to, modnn(s[i - j] + lambda[j]));
			}
		}
		omega[i] = symbol_get(index_of, tmp);
	}

	/*
	 * Compute error values in poly-form. num1 = omega(inv(X(l))), num2 =
	 * inv(X(l))**(FCR-1) and den = lambda_pr(inv(X(l))) all in poly-form
	 */
	corrections = 0;
	for (j = count - 1; j >= 0; j--) {
		num1 = 0;
		for (i = deg_omega; i >= 0; i--) {
			if (omega[i] != SYMBOL_MASK) {
				num1  ^= symbol_get(alpha_to, modnn(omega[i] + i * root[j]));
			}
		}
		num2 = symbol_get(alpha_to, modnn(root[j] * (FCS - 1) + TOTAL_SYMBOL_COUNT));
		den = 0;

		/* lambda[i+1] for i even is the formal derivative lambda_pr of lambda[i] */
		for (i = MIN(deg_lambda, PARITY_SYMBOL_COUNT - 1) & ~1; i >= 0; i -= 2) {
			if (lambda[i + 1] != SYMBOL_MASK) {
				den ^= symbol_get(alpha_to, modnn(lambda[i + 1] + i * root[j]));
			}
		}

		/* Apply error to data */
		if ((num1 != 0)) {
			if (corrections >= RS_MAX_ERRORS) {
				return -8
			}
			offsets[corrections] = symbol_address_offset(data, loc[j]);
			values[corrections++] = symbol_get(alpha_to, modnn(symbol_get(index_of, num1) + symbol_get(index_of, num2) + TOTAL_SYMBOL_COUNT - symbol_get(index_of, den)));
/*			symbol_put(data, loc[j], symbol_get(data, loc[j]) ^ symbol_get(alpha_to, modnn(symbol_get(index_of, num1) + symbol_get(index_of, num2) + TOTAL_SYMBOL_COUNT - symbol_get(index_of, den))));
 */		}
	}

	return corrections;
