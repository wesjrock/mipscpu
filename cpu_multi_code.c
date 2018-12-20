/*
NOMES DOS PARTICIPANTES:

    SSC0112 - ORGANIZAÇÃO DE COMPUTADORES DIGITAIS I

        2º TRABALHO - Simulador de uma CPU MIPS Multiciclo de 32 bits

    Weslei Renato de Lima  NºUSP: 6511258
    Wesley Tiozzo          NºUSP: 8077925
    Andreas Munte Foerster NºUSP: 7143997
    Gabriel Rodrigues do Prado Rossales NºUSP: 6608843
*/

/*
Função auxiliar do procedimento UnidadeControle().
Obtém os sinais de controle a partir de uma microinstrução
*/
void obter_sinais_controle(int microinstrucao, int *sc)
{
    int RegDst_bit, RegWrite_bit, IsZero_bit, ALUSrcA_bit, ALUSrcB0_bit, ALUSrcB1_bit,
        ALUop0_bit, ALUop1_bit, ALUop2_bit, PCSource0_bit, PCSource1_bit, PCWriteCond_bit,
        PCWrite_bit, IorD_bit, MemRead_bit, MemWrite_bit, MemtoReg0_bit, MemtoReg1_bit,
        IRWrite_bit, ControleSeq0_bit, ControleSeq1_bit;

    //Selecionando e posicionando o bit RegDst:
    RegDst_bit = microinstrucao & 0x00002000; //aplicando a máscara
    RegDst_bit = RegDst_bit >> 13;

    //Selecionando e posicionando o bit RegWrite:
    RegWrite_bit = microinstrucao & 0x00004000;
    RegWrite_bit = RegWrite_bit >> 13;

    //Selecionando e posicionando o bit IsZero:
    IsZero_bit = microinstrucao & 0x00000004;

    //Selecionando e posicionando o bit ALUSrcA:
    ALUSrcA_bit = microinstrucao & 0x00020000;
    ALUSrcA_bit = ALUSrcA_bit >> 14;

    //Selecionando e posicionando o bit ALUSrcB0:
    ALUSrcB0_bit = microinstrucao & 0x00008000;
    ALUSrcB0_bit = ALUSrcB0_bit >> 11;

    //Selecionando e posicionando o bit ALUSrcB1:
    ALUSrcB1_bit = microinstrucao & 0x00010000;
    ALUSrcB1_bit = ALUSrcB1_bit >> 11;

    //Selecionando e posicionando o bit ALUop0:
    ALUop0_bit = microinstrucao & 0x00040000;
    ALUop0_bit = ALUop0_bit >> 12;

    //Selecionando e posicionando o bit ALUop1:
    ALUop1_bit = microinstrucao & 0x00080000;
    ALUop1_bit = ALUop1_bit >> 12;

    //Selecionando e posicionando o bit ALUop2:
    ALUop2_bit = microinstrucao & 0x00100000;
    ALUop2_bit = ALUop2_bit >> 12;

    //Selecionando e posicionando o bit PCSource0:
    PCSource0_bit = microinstrucao & 0x00000008;
    PCSource0_bit = PCSource0_bit << 6;

    //Selecionando e posicionando o bit PCSource1:
    PCSource1_bit = microinstrucao & 0x00000010;
    PCSource1_bit = PCSource1_bit << 6;

    //Selecionando e posicionando o bit PCWriteCond:
    PCWriteCond_bit = microinstrucao & 0x00000020;
    PCWriteCond_bit = PCWriteCond_bit << 6;

    //Selecionando e posicionando o bit PCWrite:
    PCWrite_bit = microinstrucao & 0x00000040;
    PCWrite_bit = PCWrite_bit << 6;

    //Selecionando e posicionando o bit IorD:
    IorD_bit = microinstrucao & 0x00000100;
    IorD_bit = IorD_bit << 5;

    //Selecionando e posicionando o bit MemRead:
    MemRead_bit = microinstrucao & 0x00000400;
    MemRead_bit = MemRead_bit << 4;

    //Selecionando e posicionando o bit MemWrite:
    MemWrite_bit = microinstrucao & 0x00000200;
    MemWrite_bit = MemWrite_bit << 9;

    //Selecionando e posicionando o bit MemtoReg0:
    MemtoReg0_bit = microinstrucao & 0x00000800;
    MemtoReg0_bit = MemtoReg0_bit << 5;

    //Selecionando e posicionando o bit MemtoReg1:
    MemtoReg1_bit = microinstrucao & 0x00001000;
    MemtoReg1_bit = MemtoReg1_bit << 5;

    //Selecionando e posicionando o bit IRWrite:
    IRWrite_bit = microinstrucao & 0x00000080;
    IRWrite_bit = IRWrite_bit << 11;

    //Selecionando e posicionando o bit ControleSeq0:
    ControleSeq0_bit = microinstrucao & 0x00000001;
    ControleSeq0_bit = ControleSeq0_bit << 19;

    //Selecionando e posicionando o bit ControleSeq1:
    ControleSeq1_bit = microinstrucao & 0x00000002;
    ControleSeq1_bit = ControleSeq1_bit << 19;

    //Agrupando os sinais de controle:
    *sc = (RegDst_bit | RegWrite_bit | IsZero_bit | ALUSrcA_bit | ALUSrcB0_bit | ALUSrcB1_bit
        | ALUop0_bit | ALUop1_bit | ALUop2_bit | PCSource0_bit | PCSource1_bit | PCWriteCond_bit
        | PCWrite_bit | IorD_bit | MemRead_bit | MemWrite_bit | MemtoReg0_bit | MemtoReg1_bit
        | IRWrite_bit | ControleSeq0_bit | ControleSeq1_bit);
}

/*
Função auxiliar.
Retorna e extensão de sinal de 16 bits para 32 bits de um valor passado como argumento.
*/
int sign_extend(int value)
{
    int sign_bit = ((value & 0x00008000) >> 15);

    if(sign_bit == 0)
        return value;
    else if(sign_bit == 1)
        return (value | 0xffff0000);
}

/*
Determina a operação a ser feita pela ULA com base nos sinais de controle ALUop e no código
de função da instrução armazenada no IR
*/
char alu_control(int ALUop, int function_code)
{
    char ula_op;

    switch(ALUop)
    {
        case 0:
                ula_op = 0x02;
                break;
        case 1:
                ula_op = 0x06;
                break;
        case 2:
                //tipo-r:
                if(function_code == 0x20)//add
                    ula_op = 0x02;
                else if(function_code == 0x22)//sub
                    ula_op = 0x06;
                else if(function_code == 0x2a)//slt
                    ula_op = 0x07;
                else if(function_code == 0x24)//and
                    ula_op = 0x00;
                else if(function_code == 0x25)//or
                    ula_op = 0x01;
                break;
        case 3:
                ula_op = 0x00;
                break;
        case 4:
                ula_op = 0x01;
                break;
        case 5:
                ula_op = 0x07;
                break;
    }
    return ula_op;
}

/* UC principal
void UnidadeControle(int IR, int *sc);
args de entrada:		 int IR
args de saida: 		int *sc     */
void UnidadeControle(int IR, int *sc)
{
    int micromemoria[10];
    int opcode;
    int opcodemask = 0xfc000000;
    int microinstrucao;
    int i;

    static int EndCt1, micro_pc;

    typedef struct{
        int opcode;
        int address;
    }TABELA;

    TABELA despacho1[5], despacho2[2];

    despacho1[0].opcode = 0x00;
    despacho1[1].opcode = 0x02;
    despacho1[2].opcode = 0x04;
    despacho1[3].opcode = 0x23;
    despacho1[4].opcode = 0x2b;

    despacho1[0].address = 6; //Rformat1
    despacho1[1].address = 9; //Jump1
    despacho1[2].address = 8; //BEQ1
    despacho1[3].address = 2; //Mem1
    despacho1[4].address = 2; //Mem1

    despacho2[0].opcode = 0x23;
    despacho2[1].opcode = 0x2b;

    despacho2[0].address = 3; //LW2
    despacho2[1].address = 5; //SW2

    micromemoria[0] = 0x000084c3; //Busca
    micromemoria[1] = 0x00018001;
    micromemoria[2] = 0x00030002; //Mem1
    micromemoria[3] = 0x00000503; //LW2
    micromemoria[4] = 0x00004800;
    micromemoria[5] = 0x00000300; //SW2
    micromemoria[6] = 0x000a0002; //Rformat1
    micromemoria[7] = 0x00006000;
    micromemoria[8] = 0x00060028; //BEQ1
    micromemoria[9] = 0x00000050; //Jump1

    //Processamento inicial do IR:
    if(IR == -1)
    {
        EndCt1 = 0;
        micro_pc = 0;
    }


    //Controle do sequenciamento:
    switch(EndCt1)
    {
        case 0:
                //Desvio para a microinstrução de Busca:
                microinstrucao = micromemoria[0];
                obter_sinais_controle(microinstrucao,sc);
                EndCt1 = 3;
                break;

        case 1:
                //Despache usando a tabela 1:
                opcode = IR & opcodemask;
                opcode = opcode >> 26;
                for(i = 0; i < 5; i++)
                {
                    if(opcode == despacho1[i].opcode)
                    {
                        micro_pc = despacho1[i].address;
                        break;
                    }
                }
                microinstrucao = micromemoria[micro_pc];
                obter_sinais_controle(microinstrucao,sc);
                EndCt1 = (((*sc) & 0x00180000) >> 19);
                break;

        case 2:
                //Despache usando a tabela 2:
                opcode = IR & opcodemask;
                opcode = opcode >> 26;
                for(i = 0; i < 2; i++)
                {
                    if(opcode == despacho2[i].opcode)
                    {
                        micro_pc = despacho2[i].address;
                        break;
                    }
                }
                microinstrucao = micromemoria[micro_pc];
                obter_sinais_controle(microinstrucao,sc);
                EndCt1 = (((*sc) & 0x00180000) >> 19);
                break;

        case 3:
                //Desvio para a próxima microinstrução sequencialmente:
                micro_pc++;
                microinstrucao = micromemoria[micro_pc];
                obter_sinais_controle(microinstrucao,sc);
                EndCt1 = (((*sc) & 0x00180000) >> 19);
                break;
    }
}

/* Busca da Instrucao
void Busca_Instrucao(int sc, int PC, int ALUOUT, int IR, int A, int B, int *PCnew, int *IRnew, int *MDRnew);
args de entrada:		int sc, int PC, int ALUOUT, int IR, int A, int B
args de saida: 		int *PCnew, int *IRnew, int *MDRnew     */
void Busca_Instrucao(int sc, int PC, int ALUOUT, int IR, int A, int B, int *PCnew, int *IRnew, int *MDRnew)
{
    int IorD_bit, IRWrite_bit, MemRead_bit,
        MemWrite_bit, ALUSrcA_bit, ALUSrcB, PCSource, ALUop;
    int mem_address, mem_data, mem_writedata;
    int a, b, result_ula, function_code;
    char ula_op, zero, overflow;
    int value_aux;

    //Selecionando os bits de controle:
    IorD_bit = ((sc & 0x00002000) >> 13);
    IRWrite_bit = ((sc & 0x00040000) >> 18);
    MemRead_bit = ((sc & 0x00004000) >> 14);
    MemWrite_bit = ((sc & 0x00008000) >> 15);
    ALUSrcA_bit = ((sc & 0x00000008) >> 3);
    ALUSrcB = ((sc & 0x00000030) >> 4);
    PCSource = ((sc & 0x00000600) >> 9);
    ALUop = ((sc & 0x000001c0) >> 6);

    //Controlando as unidades funcionais:
    if(IorD_bit == 0)
        mem_address = PC;
    else if(IorD_bit == 1)
        mem_address = ALUOUT;

    mem_writedata = B;

    if(MemRead_bit == 1 && MemWrite_bit == 0)
    {
        mem_data = memoria[mem_address];
        if(IRWrite_bit == 1)
            *IRnew = mem_data;
        else if(IRWrite_bit == 0)
            *IRnew = IR;
        *MDRnew = mem_data;
    }
    else
    {
        if(MemRead_bit == 0 && MemWrite_bit == 1)
        {
            memoria[mem_address] = mem_writedata;
            *IRnew = IR;
        }
    }

    if(ALUSrcA_bit == 0)
        a = PC;
    else if(ALUSrcA_bit == 1)
        a = A;

    switch(ALUSrcB)
    {
        case 0:
                b = B;
                break;
        case 1:
                b = 1;/*pela memória ser um vetor de inteiros,temos nesta simulação
                em particular o endereçamento a palavra*/
                break;
        case 2:
                value_aux = (IR & 0x0000ffff);//value_aux = IR[15..0]
                b = sign_extend(value_aux);
                break;
        case 3:
                value_aux = (IR & 0x0000ffff);//value_aux = IR[15..0]
                b = ((sign_extend(value_aux)) << 2);
                break;
    }
    function_code = (IR & 0x0000003f);
    ula_op = alu_control(ALUop, function_code);
    ula(a, b, ula_op, &result_ula, &zero, &overflow);

    switch(PCSource)
    {
        case 0:
                *PCnew = result_ula;
                break;
        case 1:
                *PCnew = ALUOUT;
                break;
        case 2:
                value_aux = (IR & 0x03ffffff); //value_aux = IR[25..0]
                *PCnew =  ((PC & 0xf000000) | (value_aux << 2)); // PC[31..28] : (IR[25..0] << 2)
                break;
    }
}

/* Decodifica Instrucao, Busca Registradores e Calcula Endereco para beq
void Decodifica_BuscaRegistrador(int sc, int IR, int PC, int A, int B, int *Anew, int *Bnew, int *ALUOUTnew);
args de entrada:		int sc, int IR, int PC, int A, int B,
args de saida: 		int *Anew, int *Bnew, int *ALUOUTnew     */
void Decodifica_BuscaRegistrador(int sc, int IR, int PC, int A, int B, int *Anew, int *Bnew, int *ALUOUTnew)
{
    int ALUop, ALUSrcA_bit, ALUSrcB;
    int read_reg1, read_reg2, value_aux;
    int a, b, result_ula, function_code;
    char ula_op, zero, overflow;

    //Selecionando os bits de controle:
    ALUop = ((sc & 0x000001c0) >> 6);
    ALUSrcA_bit = ((sc & 0x00000008) >> 3);
    ALUSrcB = ((sc & 0x00000030) >> 4);

    //Controlando as unidades funcionais:
    read_reg1 = ((IR & 0x03e00000) >> 21);
    read_reg2 = ((IR & 0x001f0000) >> 16);

    *Anew = reg[read_reg1];
    *Bnew = reg[read_reg2];

    if(ALUSrcA_bit == 0)
        a = PC;
    else if(ALUSrcA_bit == 1)
        a = A;

    switch(ALUSrcB)
    {
        case 0:
                b = B;
                break;
        case 1:
                b = 1;
                break;
        case 2:
                value_aux = (IR & 0x0000ffff);//value_aux = IR[15..0]
                b = sign_extend(value_aux);
                break;
        case 3:
                value_aux = (IR & 0x0000ffff);//value_aux = IR[15..0]
                b = ((sign_extend(value_aux)) << 2);
                break;
    }
    function_code = (IR & 0x0000003f);
    ula_op = alu_control(ALUop, function_code);
    ula(a, b, ula_op, &result_ula, &zero, &overflow);

    *ALUOUTnew = result_ula;
}

/*Executa TipoR, Calcula endereco para lw/sw e efetiva desvio condicional e incondicional
void Execucao_CalcEnd_Desvio(int sc, int A, int B, int IR, int PC, int ALUOUT, int *ALUOUTnew, int *PCnew);
args de entrada:		int sc, int A, int B, int IR, int PC, int ALUOUT
args de saida: 		 int *ALUOUTnew, int *PCnew */
void Execucao_CalcEnd_Desvio(int sc, int A, int B, int IR, int PC, int ALUOUT, int *ALUOUTnew, int *PCnew)
{
    int ALUop, ALUSrcA_bit, ALUSrcB, RegWrite_bit, RegDst_bit, MemtoReg, PCSource;
    int a, b, result_ula, function_code, value_aux, opcode;
    char ula_op, zero, overflow;

    //Selecionando os bits de controle:
    ALUop = ((sc & 0x000001c0) >> 6);
    ALUSrcA_bit = ((sc & 0x00000008) >> 3);
    ALUSrcB = ((sc & 0x00000030) >> 4);
    RegWrite_bit = ((sc & 0x00000002) >> 1);
    RegDst_bit = (sc & 0x00000001);
    MemtoReg = ((sc & 0x00030000) >> 16);
    PCSource = ((sc & 0x00000600) >> 9);

    //Controlando as unidades funcionais:

    if(ALUSrcA_bit == 0)
        a = PC;
    else if(ALUSrcA_bit == 1)
        a = A;

    switch(ALUSrcB)
    {
        case 0:
                b = B;
                break;
        case 1:
                b = 1;
                break;
        case 2:
                value_aux = (IR & 0x0000ffff);//value_aux = IR[15..0]
                b = sign_extend(value_aux);
                break;
        case 3:
                value_aux = (IR & 0x0000ffff);//value_aux = IR[15..0]
                b = ((sign_extend(value_aux)) << 2);
                break;
    }
    function_code = (IR & 0x0000003f);
    ula_op = alu_control(ALUop, function_code);
    ula(a, b, ula_op, &result_ula, &zero, &overflow);

    *ALUOUTnew = result_ula;

    switch(PCSource)
    {
        case 0:
                *PCnew = result_ula;
                break;
        case 1:
                opcode = ((IR & 0xfc000000) >> 26);
                if(opcode == 4 && zero == 1) //condição do desvio ondicional
                    *PCnew = ALUOUT;
                else
                {
                    if(opcode == 4 && zero == 0)
                    {
                        a = PC;
                        b = 1;
                        ALUop = 0;
                        ula_op = alu_control(ALUop, function_code);
                        ula(a, b, ula_op, &result_ula, &zero, &overflow);
                        *PCnew = result_ula;
                    }
                    else
                        *PCnew = ALUOUT;
                }
                break;
        case 2:
                value_aux = (IR & 0x03ffffff); //value_aux = IR[25..0]
                *PCnew =  ((PC & 0xf000000) | (value_aux << 2)); // PC[31..28] : (IR[25..0] << 2)
                break;
    }
}

/* Escreve no Bco de Regs resultado TiporR, Le memoria em lw e escreve na memoria em sw
void EscreveTipoR_AcessaMemoria(int sc, int B, int IR, int ALUOUT, int PC, int *MDRnew, int *IRnew);
args de entrada:	 int sc, int B, int IR, int ALUOUT, int PC
args de saida: 	 int *MDRnew, int *IRnew */
void EscreveTipoR_AcessaMemoria(int sc, int B, int IR, int ALUOUT, int PC, int *MDRnew, int *IRnew)
{
    int RegWrite_bit, RegDst_bit, MemtoReg, IorD_bit, IRWrite_bit,
        MemWrite_bit, MemRead_bit;
    int write_register, write_reg_data;
    int mem_address, mem_write_data, mem_data;

    //Selecionando os bits de controle:
    RegWrite_bit = ((sc & 0x00000002) >> 1);
    RegDst_bit = (sc & 0x00000001);
    MemtoReg = ((sc & 0x00030000) >> 16);
    MemRead_bit = ((sc & 0x00004000) >> 14);
    MemWrite_bit = ((sc & 0x00008000) >> 15);
    IorD_bit = ((sc & 0x00002000) >> 13);

    //Controlando as unidades funcionais:
    if(RegDst_bit == 0)
        write_register = ((IR & 0x001f0000) >> 16);
    else if(RegDst_bit == 1)
        write_register = ((IR & 0x0000f800) >> 11);

    if(IorD_bit == 0)
        mem_address = PC;
    else if(IorD_bit == 1)
        mem_address = ALUOUT;

    mem_write_data = B;

    if(MemRead_bit == 1 && MemWrite_bit == 0)
    {
        mem_data = memoria[mem_address];
        if(IRWrite_bit == 1)
            *IRnew = mem_data;
        else if(IRWrite_bit == 0)
            *IRnew = IR;
        *MDRnew = mem_data;
    }
    else
    {
        if(MemRead_bit == 0 && MemWrite_bit == 1)
        {
            memoria[mem_address] = mem_write_data;
            *IRnew = IR;
        }
    }

    if(MemtoReg == 0)
        write_reg_data = ALUOUT;
    else if(MemtoReg == 1)
        write_reg_data = mem_data;

    if(RegWrite_bit == 1)
        reg[write_register] = write_reg_data;
}

/* Escreve no Bco de Regs o resultado da leitura da memoria feita por lw
void EscreveRefMem(sort int sc, int IR, int MDR, int ALUOUT);
args de entrada:		int sc, int IR, int MDR, int ALUOUT
args de saida: 		nao ha */
void EscreveRefMem(int sc, int IR, int MDR, int ALUOUT)
{
    int RegWrite_bit, RegDst_bit, MemtoReg;
    int write_register, write_reg_data;

    //Selecionando os bits de controle:
    RegWrite_bit = ((sc & 0x00000002) >> 1);
    RegDst_bit = (sc & 0x00000001);
    MemtoReg = ((sc & 0x00030000) >> 16);

    //Controlando as unidades funcionais:
    if(RegDst_bit == 0)
        write_register = ((IR & 0x001f0000) >> 16);
    else if(RegDst_bit == 1)
        write_register = ((IR & 0x0000f800) >> 11);

    if(MemtoReg == 0)
        write_reg_data = ALUOUT;
    else if(MemtoReg == 1)
        write_reg_data = MDR;

    if(RegWrite_bit == 1)
        reg[write_register] = write_reg_data;
}
