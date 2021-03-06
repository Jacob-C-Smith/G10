;	x86 calling conventions
;	rcx/xmm0
;	rdx/xmm1
;	r8/xmm2
;	r9/xmm3
;	RTL (C)
;
;	Stack aligned on 16 bytes. 32 bytes shadow space on stack. 
;	The specified 8 registers can only be used for parameters 
;	1 through 4. For C++ classes, the hidden this parameter 
;	is the first parameter, and is passed in RCX.
;

; 
; Offset for GXRigidbody_t
;
; GXvec3_t  velocity     - 0x00
; GXvec3_t  acceleration - 0x10
; float     mass         - 0x20
; GXvec3_t* forces       - 0x28
; size_t  forcesCount  - 0x30
; bool      useGravity   - 0x34
;



_DATA SEGMENT

_DATA ENDS

_CONST SEGMENT
forceOfGravity:
    real4 0.f
    real4 0.f
    real4 0.98f
    real4 0.f
piOver180:
    real4 0.01745329238474369f
    real4 0.01745329238474369f
    real4 0.01745329238474369f
    real4 0.01745329238474369f
oneEightyOverPi:
    real4 57.295772552490234f
    real4 57.295772552490234f
    real4 57.295772552490234f
    real4 57.295772552490234f
_CONST ENDS
 
_TEXT SEGMENT

; Calculates and sets new acceleration, velocity, and positions
; from a force vector and the mass of the entity. 
;
; velocity (new) = acceleration * delta time + velocity (last)
; position (new) = velocity * delta time + position (last)
PUBLIC calculateDerivativesOfDisplacement
calculateDerivativesOfDisplacement PROC
    mov rdx, rcx                  ; Copy the pointer to the entity

    ; Dereference both of the pointers
    mov rdx, [rdx+28h]            ; rdx = transform
    mov rcx, [rcx+30h]            ; rcx = rigid body

    mov r9, rcx                   ; Copy the rigid body pointer into r9
    add r9, 28h                   ; Go to the offset for force
    mov r9, [r9]                  ; Dereference

    ; load everything in to xmm registers
    vmovaps      xmm0, [r9]	      ; xmm0  = force
    vbroadcastss xmm1, xmm1       ; xmm1  = delta time (scalar)
    vmovaps      xmm2, [rcx+20h]  ; xmm2  = mass (scalar)
    vbroadcastss xmm2, xmm2     
    vmovaps      xmm3, xmm0       ; xmm3  = acceleration 
    vdivps       xmm3, xmm3, xmm2 ; force = mass*acceleration => acceleration = force / mass
    vmovaps      xmm4, [rcx]      ; xmm4  = last velocity
    vmovaps      xmm5, [rdx]	  ; xmm5  = last position

    ; Calculate a new velocity from the old velocity, acceleration, and delta time
    vfmadd231ps  xmm4, xmm1, xmm3 ; last velocity += acceleraiton * delta time
    vmovaps      [rcx], xmm4      ; store the result

    ; Calculate a new position from the old position, velocity, and delta time 
    vfmadd231ps  xmm5, xmm1, xmm4 ; last position += last velocity * delta time 
    vmovaps      [rdx], xmm5      ; again store the resukt
    
    ; Done
    leave
    ret

calculateDerivativesOfDisplacement ENDP
END