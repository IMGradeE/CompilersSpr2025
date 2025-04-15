	section .data

msg: db "Hello, this is my string", 0
fmtstr: db "%s", 10, 0
fmtint: db "%d", 10, 0
fmtfloat: db "%f", 10, 0
fmtuintin: db "%d", 0
fmtfloatin: db "%f", 0
float1: dd 0.0
	section .text

	extern printf
	extern scanf
	global main
main:
	push rbp
	mov rbp, rsp
mov dword [rbp-4], 2

	mov esi, [rbp-4]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, [rbp-4]
add eax, [rbp-4]

mov [rbp-8], eax

	mov esi, [rbp-8]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, [rbp-4]
mov edi,[rbp-8]
mul edi

mov [rbp-12], eax

	mov esi, [rbp-12]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, [rbp-12]
sub eax, [rbp-8]

mov [rbp-16], eax

	mov esi, [rbp-16]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov edx,0
mov eax, [rbp-16]
div dword [rbp-12]

mov [rbp-20], eax

	mov esi, [rbp-20]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, [rbp-8]
mov edi,[rbp-12]
mul edi

mov [rbp-4],eax
add eax,eax

mov [rbp-24], eax

	mov esi, [rbp-24]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, [rbp-16]
sub eax, [rbp-4]

mov [rbp-28], eax

	mov esi, [rbp-28]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-32], 12

	mov esi, [rbp-32]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-36], 2

	mov esi, [rbp-36]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-40], 2

	mov esi, [rbp-40]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-44], 17

	mov esi, [rbp-44]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-48], 1542

	mov esi, [rbp-48]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-52], 1

	mov esi, [rbp-52]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-56], 29

	mov esi, [rbp-56]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-60], 42

	mov esi, [rbp-60]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-64], 42

	mov esi, [rbp-64]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-68], 42

	mov esi, [rbp-68]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-72], 42

	mov esi, [rbp-72]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, [rbp-4]
mov [rbp-76], eax

	mov esi, [rbp-76]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov [rbp-4],eax
add eax,[rbp-76]

mov [rbp-80], eax

	mov esi, [rbp-80]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov [rbp-4],eax
add eax,[rbp-8]

mov [rbp-84], eax

	mov esi, [rbp-84]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, [rbp-36]
mov edi,[rbp-40]
mul edi

mov [rbp-88], eax

	mov esi, [rbp-88]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, [rbp-8]
mov edi,[rbp-12]
mul edi

mov [rbp-4],eax
add eax,eax

mov [rbp-92], eax

	mov esi, [rbp-92]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, [rbp-76]
sub eax, [rbp-80]

mov [rbp-96], eax

	mov esi, [rbp-96]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-100], 7006652

	mov esi, [rbp-100]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov edx,0
mov eax, 100000
div dword [rbp-52]

mov [rbp-104], eax

	mov esi, [rbp-104]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-108], 1

	mov esi, [rbp-108]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-112], 10

	mov esi, [rbp-112]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-116], 9

	mov esi, [rbp-116]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov edx,0
mov eax, [rbp-4]
div dword [rbp-8]

mov eax, [rbp-12]
mov edi,[rbp-16]
mul edi

mov eax,eax
add eax,eax

mov [rbp-120], eax

	mov esi, [rbp-120]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-124], 1

mov dword [rbp-128], 2

mov dword [rbp-132], 2

mov dword [rbp-136], 3

mov eax, [rbp-128]
mov [rbp-140], eax

	mov esi, [rbp-140]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, [rbp-128]
mov [rbp-144], eax

	mov esi, [rbp-144]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov [rbp-132],eax
add eax,[rbp-136]

mov [rbp-148], eax

	mov esi, [rbp-148]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, 300
add eax,dword [rbp-4]

mov [rbp-152], eax

	mov esi, [rbp-152]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, [rbp-148]
sub eax, [rbp-8]

mov eax, eax
add eax,18

mov [rbp-156], eax

	mov esi, [rbp-156]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-160], 1337

	mov esi, [rbp-160]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-164], 5

	mov esi, [rbp-164]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov [rbp-4],eax
add eax,[rbp-8]

mov [rbp-168], eax

	mov esi, [rbp-168]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-172], -42

	mov esi, [rbp-172]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-176], -42

	mov esi, [rbp-176]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-180], -42

	mov esi, [rbp-180]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

NEG dword [rbp-4]

mov eax, [rbp-4]
mov [rbp-184], eax

	mov esi, [rbp-184]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, -1
mov edi,dword [rbp-4]
mul edi

mov edx,0
mov eax, eax
mov esi,-1
div esi

mov [rbp-188], eax

	mov esi, [rbp-188]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-192], -1

	mov esi, [rbp-192]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-196], -1

	mov esi, [rbp-196]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-200], 5

	mov esi, [rbp-200]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-204], 5

	mov esi, [rbp-204]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-208], -5

	mov esi, [rbp-208]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-212], 1

	mov esi, [rbp-212]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-216], 1

	mov esi, [rbp-216]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-220], 1

	mov esi, [rbp-220]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-224], 1

	mov esi, [rbp-224]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-228], 1

	mov esi, [rbp-228]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-232], 1

	mov esi, [rbp-232]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-236], 1

	mov esi, [rbp-236]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-240], 5

	mov esi, [rbp-240]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-244], 5

	mov esi, [rbp-244]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-248], -3

	mov esi, [rbp-248]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-252], -4

	mov esi, [rbp-252]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-256], -4

	mov esi, [rbp-256]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-260], 33

	mov esi, [rbp-260]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-264], 5

	mov esi, [rbp-264]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-268], -1

	mov esi, [rbp-268]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-272], 100

	mov esi, [rbp-272]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

NEG dword [rbp-124]

mov eax, 2
sub eax,dword [rbp-124]

mov [rbp-276], eax

	mov esi, [rbp-276]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

NEG dword [rbp-124]

mov eax, 2
add eax,dword [rbp-124]

mov [rbp-280], eax

	mov esi, [rbp-280]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-284], -2

	mov esi, [rbp-284]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-288], -2

	mov esi, [rbp-288]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-292], -2

	mov esi, [rbp-292]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-296], 7

	mov esi, [rbp-296]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-300], 4

mov dword [rbp-304], 3

mov dword [rbp-308], 2

mov dword [rbp-312], 1

mov eax, [rbp-300]
mov edi,[rbp-304]
mul edi

mov edx,0
mov eax, eax
div dword [rbp-308]

mov eax,eax
add eax,[rbp-312]

mov [rbp-316], eax

	mov esi, [rbp-316]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov eax, 6
add eax,dword [rbp-312]

mov [rbp-320], eax

	mov esi, [rbp-320]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-324], 31

	mov esi, [rbp-324]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov edx,0
mov eax, dword [rbp-4]
mov esi,2
div esi

mov eax, 7
add eax,eax

mov eax, 20
add eax,eax

mov [rbp-328], eax

	mov esi, [rbp-328]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-332], 1234

	mov esi, [rbp-332]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

mov dword [rbp-336], 314

	mov esi, [rbp-336]
	mov edi, fmtint
	mov eax, 0 ; I don't know if this is necessary
	call printf

pop rbp ; restore stack base pointer
mov rax, 60
xor rdi, rdi
syscall ; exit cleanly