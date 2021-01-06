fin = open("opti_icg2.txt","r")
fout = open("opti_icg3.txt", "w")

i = 0
label_flag = 1
list_of_lines = fin.readlines()
dictValues = dict()
constantFoldedList = []
print("------------------------------------------------Pass 2------------------------------------------------")
print("-------------------------------------------Constant Folding-------------------------------------------\n")
while(i < len(list_of_lines)):
    line = list_of_lines[i].strip("\n")
    i += 1
    op,arg1,arg2,res = line.split()
    if(op in ["+", "-", "*", "/", ">", "<", "<=", ">=", "==", "!="]):
        if(arg1.isdigit() and arg2.isdigit()):
            result = str(int(eval(arg1 + op + arg2)))
            dictValues[res] = result
            constantFoldedList.append(["=",result,"NULL",res])
            print("", "=", result, "NULL", res, sep="\t")
        elif(arg1.isdigit()):
            if(arg2 in dictValues):
                result = str(int(eval(arg1 + op + dictValues[arg2])))
                dictValues[res] = result
                constantFoldedList.append(["=",result,"NULL",res])
                print("", "=", result, "NULL", res, sep="\t")
            else:
                constantFoldedList.append([op,arg1,arg2,res])
                print("", op, arg1, arg2, res, sep="\t")
        elif(arg2.isdigit()):
            if(arg1 in dictValues):
                result = str(int(eval(dictValues[arg1] + op + arg2)))
                dictValues[res] = result
                constantFoldedList.append(["=",result,"NULL",res])
                print("", "=", result, "NULL", res, sep="\t")
            else:
                constantFoldedList.append([op,arg1,arg2,res])
                print("", op, arg1, arg2, res, sep="\t")
        else:
            flag1=0
            flag2=0
            arg1Res = arg1
            if(arg1 in dictValues):
                arg1Res = str(dictValues[arg1])
                flag1 = 1
            arg2Res = arg2
            if(arg2 in dictValues):
                arg2Res = str(dictValues[arg2])
                flag2 = 1
            if(flag1==1 and flag2==1):
                result = str(int(eval(arg1Res + op + arg2Res)))
                dictValues[res] = result
                constantFoldedList.append(["=",result,"NULL",res])
                print("", "=", result, "NULL", res, sep="\t")
            else:
                constantFoldedList.append([op,arg1Res,arg2Res,res])
                print("", op, arg1Res, arg2Res, res, sep="\t")
    elif(op=="="):
        if(arg1.isdigit()):
            dictValues[res]=arg1
            constantFoldedList.append(["=",arg1,"NULL",res])
            print("", "=", arg1, "NULL", res, sep="\t")
        else:
            if(arg1 in dictValues):
                dictValues[res]=dictValues[arg1]
                constantFoldedList.append(["=",dictValues[arg1],"NULL",res])
                print("", "=", arg1, "NULL", res, sep="\t")
                if (arg1[0] == "t"):
                    dictValues.pop(arg1)
            else:
                constantFoldedList.append(["=",arg1,"NULL",res])
                print("", "=", arg1, "NULL", res, sep="\t")
    elif(op == "ifFalse"):
        if(arg1 in dictValues):
            constantFoldedList.append([op, dictValues[arg1], arg2, res])
            print("", op, dictValues[arg1], arg2, res, sep="\t")
        else:
            constantFoldedList.append([op,arg1,arg2,res])
            print("", op, arg1, arg2, res, sep="\t")
    else:
        constantFoldedList.append([op,arg1,arg2,res])
        print("", op, arg1, arg2, res, sep="\t")

print()
print("--------------------------------------Constant folded expression--------------------------------------\n")
for i in constantFoldedList:
    if(i[0]=="="):
        print("", i[3], i[0], i[1], sep="\t")
        print(i[3], i[0], i[1], file=fout)
    elif(i[0] in ["+","-","*","/","==","<=","<",">",">="]):
        print("", i[3], "=", i[1], i[0], i[2], sep="\t")
        print(i[3], "=", i[1], i[0], i[2],file=fout)
    elif(i[0] in ["ifFalse","goto","Label","not"]):
        if(i[0]=="ifFalse"):
            print("", i[0], i[1], "goto", i[3], sep="\t")
            print(i[0], i[1], "goto", i[3], file=fout)
        if(i[0]=="goto"):
            print(i[0],i[3])
        if(i[0]=="Label"):
            print("", i[3], ":", sep="\t")
            print(i[3], ":",file=fout)
        if(i[0]=="not"):
            print(i[3],"=",i[0],i[1])
fin.close()
fout.close()
print()
print()
print("--------------------------------------Assembly Code--------------------------------------\n")