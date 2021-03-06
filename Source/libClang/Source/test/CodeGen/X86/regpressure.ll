; REQUIRES: asserts
;; Both functions in this testcase should codegen to the same function, and
;; neither of them should require spilling anything to the stack.

; RUN: llc < %s -march=x86 -stats 2>&1 | \
; RUN:   not grep "Number of register spills"

;; This can be compiled to use three registers if the loads are not
;; folded into the multiplies, 2 registers otherwise.

define i32 @regpressure1(i32* %P) {
	%A = load i32* %P		; <i32> [#uses=1]
	%Bp = getelementptr i32* %P, i32 1		; <i32*> [#uses=1]
	%B = load i32* %Bp		; <i32> [#uses=1]
	%s1 = mul i32 %A, %B		; <i32> [#uses=1]
	%Cp = getelementptr i32* %P, i32 2		; <i32*> [#uses=1]
	%C = load i32* %Cp		; <i32> [#uses=1]
	%s2 = mul i32 %s1, %C		; <i32> [#uses=1]
	%Dp = getelementptr i32* %P, i32 3		; <i32*> [#uses=1]
	%D = load i32* %Dp		; <i32> [#uses=1]
	%s3 = mul i32 %s2, %D		; <i32> [#uses=1]
	%Ep = getelementptr i32* %P, i32 4		; <i32*> [#uses=1]
	%E = load i32* %Ep		; <i32> [#uses=1]
	%s4 = mul i32 %s3, %E		; <i32> [#uses=1]
	%Fp = getelementptr i32* %P, i32 5		; <i32*> [#uses=1]
	%F = load i32* %Fp		; <i32> [#uses=1]
	%s5 = mul i32 %s4, %F		; <i32> [#uses=1]
	%Gp = getelementptr i32* %P, i32 6		; <i32*> [#uses=1]
	%G = load i32* %Gp		; <i32> [#uses=1]
	%s6 = mul i32 %s5, %G		; <i32> [#uses=1]
	%Hp = getelementptr i32* %P, i32 7		; <i32*> [#uses=1]
	%H = load i32* %Hp		; <i32> [#uses=1]
	%s7 = mul i32 %s6, %H		; <i32> [#uses=1]
	%Ip = getelementptr i32* %P, i32 8		; <i32*> [#uses=1]
	%I = load i32* %Ip		; <i32> [#uses=1]
	%s8 = mul i32 %s7, %I		; <i32> [#uses=1]
	%Jp = getelementptr i32* %P, i32 9		; <i32*> [#uses=1]
	%J = load i32* %Jp		; <i32> [#uses=1]
	%s9 = mul i32 %s8, %J		; <i32> [#uses=1]
	ret i32 %s9
}

define i32 @regpressure2(i32* %P) {
	%A = load i32* %P		; <i32> [#uses=1]
	%Bp = getelementptr i32* %P, i32 1		; <i32*> [#uses=1]
	%B = load i32* %Bp		; <i32> [#uses=1]
	%Cp = getelementptr i32* %P, i32 2		; <i32*> [#uses=1]
	%C = load i32* %Cp		; <i32> [#uses=1]
	%Dp = getelementptr i32* %P, i32 3		; <i32*> [#uses=1]
	%D = load i32* %Dp		; <i32> [#uses=1]
	%Ep = getelementptr i32* %P, i32 4		; <i32*> [#uses=1]
	%E = load i32* %Ep		; <i32> [#uses=1]
	%Fp = getelementptr i32* %P, i32 5		; <i32*> [#uses=1]
	%F = load i32* %Fp		; <i32> [#uses=1]
	%Gp = getelementptr i32* %P, i32 6		; <i32*> [#uses=1]
	%G = load i32* %Gp		; <i32> [#uses=1]
	%Hp = getelementptr i32* %P, i32 7		; <i32*> [#uses=1]
	%H = load i32* %Hp		; <i32> [#uses=1]
	%Ip = getelementptr i32* %P, i32 8		; <i32*> [#uses=1]
	%I = load i32* %Ip		; <i32> [#uses=1]
	%Jp = getelementptr i32* %P, i32 9		; <i32*> [#uses=1]
	%J = load i32* %Jp		; <i32> [#uses=1]
	%s1 = mul i32 %A, %B		; <i32> [#uses=1]
	%s2 = mul i32 %s1, %C		; <i32> [#uses=1]
	%s3 = mul i32 %s2, %D		; <i32> [#uses=1]
	%s4 = mul i32 %s3, %E		; <i32> [#uses=1]
	%s5 = mul i32 %s4, %F		; <i32> [#uses=1]
	%s6 = mul i32 %s5, %G		; <i32> [#uses=1]
	%s7 = mul i32 %s6, %H		; <i32> [#uses=1]
	%s8 = mul i32 %s7, %I		; <i32> [#uses=1]
	%s9 = mul i32 %s8, %J		; <i32> [#uses=1]
	ret i32 %s9
}

define i32 @regpressure3(i16* %P, i1 %Cond, i32* %Other) {
	%A = load i16* %P		; <i16> [#uses=1]
	%Bp = getelementptr i16* %P, i32 1		; <i16*> [#uses=1]
	%B = load i16* %Bp		; <i16> [#uses=1]
	%Cp = getelementptr i16* %P, i32 2		; <i16*> [#uses=1]
	%C = load i16* %Cp		; <i16> [#uses=1]
	%Dp = getelementptr i16* %P, i32 3		; <i16*> [#uses=1]
	%D = load i16* %Dp		; <i16> [#uses=1]
	%Ep = getelementptr i16* %P, i32 4		; <i16*> [#uses=1]
	%E = load i16* %Ep		; <i16> [#uses=1]
	%Fp = getelementptr i16* %P, i32 5		; <i16*> [#uses=1]
	%F = load i16* %Fp		; <i16> [#uses=1]
	%Gp = getelementptr i16* %P, i32 6		; <i16*> [#uses=1]
	%G = load i16* %Gp		; <i16> [#uses=1]
	%Hp = getelementptr i16* %P, i32 7		; <i16*> [#uses=1]
	%H = load i16* %Hp		; <i16> [#uses=1]
	%Ip = getelementptr i16* %P, i32 8		; <i16*> [#uses=1]
	%I = load i16* %Ip		; <i16> [#uses=1]
	%Jp = getelementptr i16* %P, i32 9		; <i16*> [#uses=1]
	%J = load i16* %Jp		; <i16> [#uses=1]
	%A.upgrd.1 = sext i16 %A to i32		; <i32> [#uses=1]
	%B.upgrd.2 = sext i16 %B to i32		; <i32> [#uses=1]
	%D.upgrd.3 = sext i16 %D to i32		; <i32> [#uses=1]
	%C.upgrd.4 = sext i16 %C to i32		; <i32> [#uses=1]
	%E.upgrd.5 = sext i16 %E to i32		; <i32> [#uses=1]
	%F.upgrd.6 = sext i16 %F to i32		; <i32> [#uses=1]
	%G.upgrd.7 = sext i16 %G to i32		; <i32> [#uses=1]
	%H.upgrd.8 = sext i16 %H to i32		; <i32> [#uses=1]
	%I.upgrd.9 = sext i16 %I to i32		; <i32> [#uses=1]
	%J.upgrd.10 = sext i16 %J to i32		; <i32> [#uses=1]
	%s1 = add i32 %A.upgrd.1, %B.upgrd.2		; <i32> [#uses=1]
	%s2 = add i32 %C.upgrd.4, %s1		; <i32> [#uses=1]
	%s3 = add i32 %D.upgrd.3, %s2		; <i32> [#uses=1]
	%s4 = add i32 %E.upgrd.5, %s3		; <i32> [#uses=1]
	%s5 = add i32 %F.upgrd.6, %s4		; <i32> [#uses=1]
	%s6 = add i32 %G.upgrd.7, %s5		; <i32> [#uses=1]
	%s7 = add i32 %H.upgrd.8, %s6		; <i32> [#uses=1]
	%s8 = add i32 %I.upgrd.9, %s7		; <i32> [#uses=1]
	%s9 = add i32 %J.upgrd.10, %s8		; <i32> [#uses=1]
	ret i32 %s9
}
