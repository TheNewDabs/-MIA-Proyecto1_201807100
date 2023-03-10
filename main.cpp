#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

struct BloqueApuntadores{
    int b_pointers[16];
};

struct BloqueArchivos{
    char b_content[64];
};

struct content{
    char b_name[12];
    int b_inodo;
};

struct BloqueCarpeta{
    content b_content[4];
};

struct TablaInodo{
    int i_uid;
    int i_gid;
    int i_size;
    time_t i_atime;
    time_t i_ctime;
    time_t i_mtime;
    int i_block[15];
    char i_type;
    int i_perm;
};

struct SuperBloque{
    int s_filesystem_type;
    int s_inodes_count;
    int s_blocks_count;
    int s_free_blocks_count;
    int s_free_inodes_count;
    time_t s_mtime;
    time_t s_umtime;
    int s_mnt_count;
    int s_magic;
    int s_inode_s;
    int s_block_s;
    int s_firts_ino;
    int s_first_blo;
    int s_bm_inode_start;
    int s_bm_block_start;
    int s_inode_start;
    int s_block_start;
};

struct MountPart
{
    bool Active;
    string ID;
    string Path;
    char part_name[16];
    int part_size;
    int part_start;
};

struct EBR
{
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char part_name[16];
};

struct partition
{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];
};

struct MBR
{
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_dsk_signature;
    char mbr_fit;
    partition mbr_partition_1;
    partition mbr_partition_2;
    partition mbr_partition_3;
    partition mbr_partition_4;
};

string strLower(string);
bool LeerComando(string);
void LeerScript(string);
void CrearDisco(int, string, string, string);
void EliminarDisco(string);
void CrearParticion(int, string, string, string, string, string);
void EliminarParticion(string, string);
void MountParticion(string, string);
void UnmountParticion(string);
MountPart BuscarParticion(string, string);
string Valor(string &);
void CrearPath(string);
string getName(string);
void VerDisco(string);
void VerMounts();

MountPart ActivePart[10];

int main()
{
    for (int i = 0; i < 10; i++)
    {
        ActivePart[i].Active = false;
    }
    srand(time(NULL));
    cout << "Daniel Alejandro Barillas Soberanis" << endl;
    cout << "201807100" << endl;
    cout << "--------------Proyecto#1--------------" << endl;
    string Linea;
    do
    {
        getline(cin, Linea);
    } while (LeerComando(Linea + " "));
}

string strLower(string Text)
{
    for (int i = 0; i < Text.length(); i++)
        Text[i] = tolower(Text[i]);
    return Text;
}

bool LeerComando(string Linea)
{
    bool Error = false;
    string Aux = Valor(Linea);
    if (strLower(Aux) == "execute")
    {
        string Path = "";
        bool BPath = false;
        while (Linea != "" && !Error)
        {
            Aux = Valor(Linea);
            if (strLower(Aux) == ">path")
            {
                Path = Valor(Linea);
                BPath = true;
            }
            else
            {
                Error = true;
                cout << "Error, Parametro desconocido" << endl;
            }
        }
        if (BPath && !Error)
        {
            LeerScript(Path);
        }
        else if (!Error)
        {
            cout << "Error, Faltan parametros" << endl;
        }
    }
    else if (strLower(Aux) == "mkdisk")
    {
        int Size;
        string Path, Fit = "FF", Unit = "M";
        bool BSize = false, BPath = false;
        while (Linea != "" && !Error)
        {
            Aux = Valor(Linea);
            if (strLower(Aux) == ">size")
            {
                Size = stoi(Valor(Linea));
                if (Size <= 0)
                {
                    Error = true;
                    cout << "Error, El tamaño del disco debe ser mayor a 0" << endl;
                }
                BSize = true;
            }
            else if (strLower(Aux) == ">path")
            {
                Path = Valor(Linea);
                BPath = true;
            }
            else if (strLower(Aux) == ">fit")
            {
                Fit = Valor(Linea);
                if (!(strLower(Fit) == "bf" || strLower(Fit) == "ff" || strLower(Fit) == "wf"))
                {
                    Error = true;
                    cout << "Error, Valor invalido o no soportado" << endl;
                }
            }
            else if (strLower(Aux) == ">unit")
            {
                Unit = Valor(Linea);
                if (!(strLower(Unit) == "k" || strLower(Unit) == "m"))
                {
                    Error = true;
                    cout << "Error, Valor invalido o no soportado" << endl;
                }
            }
            else
            {
                Error = true;
                cout << "Error, Parametro desconocido" << endl;
            }
        }
        if (BSize && BPath && !Error)
        {
            CrearDisco(Size, Path, Fit, Unit);
        }
        else if (!Error)
        {
            cout << "Error, Faltan parametros" << endl;
        }
    }
    else if (strLower(Aux) == "rmdisk")
    {
        string Path = "";
        bool BPath = false;
        while (Linea != "" && !Error)
        {
            Aux = Valor(Linea);
            if (strLower(Aux) == ">path")
            {
                Path = Valor(Linea);
                BPath = true;
            }
            else
            {
                Error = true;
                cout << "Error, Parametro desconocido" << endl;
            }
        }
        if (BPath && !Error)
        {
            EliminarDisco(Path);
        }
        else if (!Error)
        {
            cout << "Error, Faltan parametros" << endl;
        }
    }
    else if (strLower(Aux) == "fdisk")
    {
        int Size, Add = 0;
        string Path, Name, Unit = "k", Type = "p", Fit = "wf", Delete = "";
        bool BSize = false, BPath = false, BName = false, BDelete = false;
        while (Linea != "" && !Error)
        {
            Aux = Valor(Linea);
            if (strLower(Aux) == ">size")
            {
                Size = stoi(Valor(Linea));
                if (Size <= 0)
                {
                    Error = true;
                    cout << "Error, El tamaño del disco debe ser mayor a 0" << endl;
                }
                BSize = true;
            }
            else if (strLower(Aux) == ">path")
            {
                Path = Valor(Linea);
                BPath = true;
            }
            else if (strLower(Aux) == ">name")
            {
                Name = Valor(Linea);
                if (Name.length() > 16)
                {
                    Error = true;
                    cout << "Error, El nombre debe ser menor a 16 caracteres";
                }
                BName = true;
            }
            else if (strLower(Aux) == ">unit")
            {
                Unit = Valor(Linea);
                if (!(strLower(Unit) == "b" || strLower(Unit) == "k" || strLower(Unit) == "m"))
                {
                    Error = true;
                    cout << "Error, Valor invalido o no soportado" << endl;
                }
            }
            else if (strLower(Aux) == ">type")
            {
                Type = Valor(Linea);
                if (!(strLower(Type) == "p" || strLower(Type) == "e" || strLower(Type) == "l"))
                {
                    Error = true;
                    cout << "Error, Valor invalido o no soportado" << endl;
                }
            }
            else if (strLower(Aux) == ">fit")
            {
                Fit = Valor(Linea);
                if (!(strLower(Fit) == "bf" || strLower(Fit) == "ff" || strLower(Fit) == "wf"))
                {
                    Error = true;
                    cout << "Error, Valor invalido o no soportado" << endl;
                }
            }
            else if (strLower(Aux) == ">delete")
            {
                Delete = Valor(Linea);
                if (strLower(Delete) != "full")
                {
                    Error = true;
                    cout << "Error, Opcion invalida o no soportada" << endl;
                }
                else
                {
                    BDelete = true;
                }
            }
            else if (strLower(Aux) == ">add")
            {
                Add = stoi(Valor(Linea));
                if (Add <= 0)
                {
                    Error = true;
                    cout << "Error, El tamaño del disco debe ser mayor a 0" << endl;
                }
            }
            else
            {
                Error = true;
                cout << "Error, Parametro desconocido" << endl;
            }
        }
        if (BSize && BPath && BName && !Error)
        {
            CrearParticion(Size, Path, Name, Unit, Type, Fit);
        }
        else if (BDelete && BPath && BName && !Error)
        {
            EliminarParticion(Path, Name);
        }
        else if (!Error)
        {
            cout << "Error, Faltan parametros" << endl;
        }
    }
    else if (strLower(Aux) == "mount")
    {
        string Path, Name;
        bool BPath = false, BName = false;
        while (Linea != "" && !Error)
        {
            Aux = Valor(Linea);
            if (strLower(Aux) == ">path")
            {
                Path = Valor(Linea);
                BPath = true;
            }
            else if (strLower(Aux) == ">name")
            {
                Name = Valor(Linea);
                if (Name.length() > 16)
                {
                    Error = true;
                    cout << "Error, El nombre debe ser menor a 16 caracteres";
                }
                BName = true;
            }
            else
            {
                Error = true;
                cout << "Error, Parametro desconocido" << endl;
            }
        }
        if (BPath && BName && !Error)
        {
            MountParticion(Path, Name);
        }
        else if (!Error)
        {
            cout << "Error, Faltan parametros" << endl;
        }
    }
    else if (strLower(Aux) == "unmount")
    {
        string ID;
        bool BID = false;
        while (Linea != "" && !Error)
        {
            Aux = Valor(Linea);
            if (strLower(Aux) == ">id")
            {
                ID = Valor(Linea);
                BID = true;
            }
            else
            {
                Error = true;
                cout << "Error, Parametro desconocido" << endl;
            }
        }
        if (BID && !Error)
        {
            UnmountParticion(ID);
        }
        else if (!Error)
        {
            cout << "Error, Faltan parametros" << endl;
        }
    }
    else if (strLower(Aux) == "seedisk")
    {
        string Path = "";
        bool BPath = false;
        while (Linea != "" && !Error)
        {
            Aux = Valor(Linea);
            if (strLower(Aux) == ">path")
            {
                Path = Valor(Linea);
                BPath = true;
            }
            else
            {
                Error = true;
                cout << "Error, Parametro desconocido" << endl;
            }
        }
        if (BPath && !Error)
        {
            VerDisco(Path);
        }
        else if (!Error)
        {
            cout << "Error, Faltan parametros" << endl;
        }
    }
    else if (strLower(Aux) == "seemounts")
    {
        VerMounts();
    }
    else if (strLower(Aux) == "rep")
    {
    }
    else if (strLower(Aux) == "exit")
    {
        return false;
    }
    else
    {
        cout << "Error, Comando desconocido" << endl;
    }
    return true;
}

void CrearDisco(int Size, string Path, string Fit, string Unit)
{
    MBR NewMBR;
    if (strLower(Unit) == "k")
    {
        NewMBR.mbr_tamano = Size * 1024;
    }
    else
    {
        NewMBR.mbr_tamano = Size * 1024 * 1024;
    }
    time(&NewMBR.mbr_fecha_creacion);
    NewMBR.mbr_dsk_signature = rand() % 10;
    if (strLower(Fit) == "bf")
    {
        NewMBR.mbr_fit = 'b';
    }
    else if (strLower(Fit) == "ff")
    {
        NewMBR.mbr_fit = 'f';
    }
    else
    {
        NewMBR.mbr_fit = 'w';
    }
    NewMBR.mbr_partition_1.part_status = 'D';
    NewMBR.mbr_partition_2.part_status = 'D';
    NewMBR.mbr_partition_3.part_status = 'D';
    NewMBR.mbr_partition_4.part_status = 'D';
    NewMBR.mbr_partition_1.part_type = ' ';
    NewMBR.mbr_partition_2.part_type = ' ';
    NewMBR.mbr_partition_3.part_type = ' ';
    NewMBR.mbr_partition_4.part_type = ' ';
    NewMBR.mbr_partition_1.part_fit = ' ';
    NewMBR.mbr_partition_2.part_fit = ' ';
    NewMBR.mbr_partition_3.part_fit = ' ';
    NewMBR.mbr_partition_4.part_fit = ' ';
    NewMBR.mbr_partition_1.part_start = -1;
    NewMBR.mbr_partition_2.part_start = -1;
    NewMBR.mbr_partition_3.part_start = -1;
    NewMBR.mbr_partition_4.part_start = -1;
    NewMBR.mbr_partition_1.part_size = -1;
    NewMBR.mbr_partition_2.part_size = -1;
    NewMBR.mbr_partition_3.part_size = -1;
    NewMBR.mbr_partition_4.part_size = -1;
    strcpy(NewMBR.mbr_partition_1.part_name, "");
    strcpy(NewMBR.mbr_partition_2.part_name, "");
    strcpy(NewMBR.mbr_partition_3.part_name, "");
    strcpy(NewMBR.mbr_partition_4.part_name, "");
    char data[NewMBR.mbr_tamano / 1024] = {0};
    CrearPath(Path);
    fstream file(Path, ios::binary | ios::out | ios::trunc);
    if (file.is_open())
    {
        for (int i = 0; i < 1024; i++)
        {
            file.write(data, 1024);
        }
        file.seekg(0);
        file.write(reinterpret_cast<char *>(&NewMBR), sizeof(NewMBR));
        cout << "Archivo creado" << endl;
    }
    else
    {
        cout << "Error, no se pudo abrir el archivo" << endl;
    }
}

void EliminarDisco(string Path)
{
    ifstream file(Path, ios::binary);
    if (file.is_open())
    {
        file.close();
        bool Salida = false, Eliminar = false;
        while (!Salida)
        {
            cout << "¿Esta seguro de eliminar este disco(s/n)?:";
            string Aux = "";
            getline(cin, Aux);
            if (strLower(Aux) == "s")
            {
                Eliminar = true;
                Salida = true;
            }
            else if (strLower(Aux) == "n")
            {
                Salida = true;
            }
            else
            {
                cout << "Error, opcion no permitida" << endl;
            }
        }
        if (Eliminar)
        {
            remove(Path.c_str());
            cout << "Archivo Eliminado con exito" << endl;
        }
    }
    else
    {
        file.close();
        cout << "Error, archvo no encontrado" << endl;
    }
}

void CrearParticion(int Size, string Path, string Name, string Unit, string Type, string Fit)
{
    if (strLower(Unit) == "k")
    {
        Size = Size * 1024;
    }
    else if (strLower(Unit) == "m")
    {
        Size = Size * 1024 * 1024;
    }
    MBR MBRDsk;
    fstream file(Path, ios::binary | ios::in | ios::out);
    if (file.is_open())
    {
        file.read(reinterpret_cast<char *>(&MBRDsk), sizeof(MBRDsk));
        if (strLower(Type) == "p" || strLower(Type) == "e")
        {
            if (strLower(Type) == "p" || (MBRDsk.mbr_partition_1.part_type != 'E' && MBRDsk.mbr_partition_2.part_type != 'E' && MBRDsk.mbr_partition_3.part_type != 'E' && MBRDsk.mbr_partition_4.part_type != 'E'))
            {
                if (MBRDsk.mbr_partition_4.part_status == 'D')
                {
                    bool Registrado = true;
                    int RegistrarEn = 5;
                    if (strLower(Fit) == "bf")
                    {
                        if (MBRDsk.mbr_partition_1.part_status == 'D')
                        {
                            if ((MBRDsk.mbr_tamano - sizeof(MBRDsk)) >= Size)
                            {
                                RegistrarEn = 1;
                            }
                        }
                        else
                        {
                            int MinSize = MBRDsk.mbr_tamano;
                            if (MBRDsk.mbr_partition_1.part_start - sizeof(MBRDsk) >= Size)
                            {
                                MinSize = MBRDsk.mbr_partition_1.part_start - sizeof(MBRDsk);
                                RegistrarEn = 1;
                            }
                            if (MBRDsk.mbr_partition_2.part_status == 'D')
                            {
                                if (MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size) >= Size)
                                {
                                    if (MinSize > MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size))
                                    {
                                        RegistrarEn = 2;
                                    }
                                }
                            }
                            else
                            {
                                if (MBRDsk.mbr_partition_2.part_start - (MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size) >= Size)
                                {
                                    if (MinSize > (MBRDsk.mbr_partition_2.part_start - (MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size)))
                                    {
                                        MinSize = MBRDsk.mbr_partition_2.part_start - (MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size);
                                        RegistrarEn = 2;
                                    }
                                }
                                if (MBRDsk.mbr_partition_3.part_status == 'D')
                                {
                                    if (MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size) >= Size)
                                    {
                                        if (MinSize > MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size))
                                        {
                                            RegistrarEn = 3;
                                        }
                                    }
                                }
                                else
                                {
                                    if (MBRDsk.mbr_partition_3.part_start - (MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size) >= Size)
                                    {
                                        if (MinSize > (MBRDsk.mbr_partition_3.part_start - (MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size)))
                                        {
                                            MinSize = MBRDsk.mbr_partition_3.part_start - (MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size);
                                            RegistrarEn = 3;
                                        }
                                    }
                                    if (MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_3.part_start + MBRDsk.mbr_partition_3.part_size) >= Size)
                                    {
                                        if (MinSize > MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_3.part_start + MBRDsk.mbr_partition_3.part_size))
                                        {
                                            RegistrarEn = 4;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (strLower(Fit) == "ff")
                    {
                        if (MBRDsk.mbr_partition_1.part_status == 'D')
                        {
                            if ((MBRDsk.mbr_tamano - sizeof(MBRDsk)) >= Size)
                            {
                                RegistrarEn = 1;
                            }
                        }
                        else
                        {
                            if (MBRDsk.mbr_partition_1.part_start - sizeof(MBRDsk) >= Size)
                            {
                                RegistrarEn = 1;
                            }
                            else
                            {
                                if (MBRDsk.mbr_partition_2.part_status == 'D')
                                {
                                    if (MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size) >= Size)
                                    {
                                        RegistrarEn = 2;
                                    }
                                }
                                else
                                {
                                    if (MBRDsk.mbr_partition_2.part_start - (MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size) >= Size)
                                    {
                                        RegistrarEn = 2;
                                    }
                                    else
                                    {
                                        if (MBRDsk.mbr_partition_3.part_status == 'D')
                                        {
                                            if (MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size) >= Size)
                                            {
                                                RegistrarEn = 3;
                                            }
                                        }
                                        else
                                        {
                                            if (MBRDsk.mbr_partition_3.part_start - (MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size) >= Size)
                                            {
                                                RegistrarEn = 3;
                                            }
                                            else
                                            {
                                                if (MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_3.part_start + MBRDsk.mbr_partition_3.part_size) >= Size)
                                                {
                                                    RegistrarEn = 4;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (MBRDsk.mbr_partition_1.part_status == 'D')
                        {
                            if ((MBRDsk.mbr_tamano - sizeof(MBRDsk)) >= Size)
                            {
                                RegistrarEn = 1;
                            }
                        }
                        else
                        {
                            int MaxSize = 0;
                            if (MBRDsk.mbr_partition_1.part_start - sizeof(MBRDsk) >= Size)
                            {
                                MaxSize = MBRDsk.mbr_partition_1.part_start - sizeof(MBRDsk);
                                RegistrarEn = 1;
                            }
                            if (MBRDsk.mbr_partition_2.part_status == 'D')
                            {
                                if (MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size) >= Size)
                                {
                                    if (MaxSize < MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size))
                                    {
                                        RegistrarEn = 2;
                                    }
                                }
                            }
                            else
                            {
                                if (MBRDsk.mbr_partition_2.part_start - (MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size) >= Size)
                                {
                                    if (MaxSize < (MBRDsk.mbr_partition_2.part_start - (MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size)))
                                    {
                                        MaxSize = MBRDsk.mbr_partition_2.part_start - (MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size);
                                        RegistrarEn = 2;
                                    }
                                }
                                if (MBRDsk.mbr_partition_3.part_status == 'D')
                                {
                                    if (MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size) >= Size)
                                    {
                                        if (MaxSize < MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size))
                                        {
                                            RegistrarEn = 3;
                                        }
                                    }
                                }
                                else
                                {
                                    if (MBRDsk.mbr_partition_3.part_start - (MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size) >= Size)
                                    {
                                        if (MaxSize < (MBRDsk.mbr_partition_3.part_start - (MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size)))
                                        {
                                            MaxSize = MBRDsk.mbr_partition_3.part_start - (MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size);
                                            RegistrarEn = 3;
                                        }
                                    }
                                    if (MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_3.part_start + MBRDsk.mbr_partition_3.part_size) >= Size)
                                    {
                                        if (MaxSize < MBRDsk.mbr_tamano - (MBRDsk.mbr_partition_3.part_start + MBRDsk.mbr_partition_3.part_size))
                                        {
                                            RegistrarEn = 4;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (RegistrarEn == 1)
                    {
                        MBRDsk.mbr_partition_4 = MBRDsk.mbr_partition_3;
                        MBRDsk.mbr_partition_3 = MBRDsk.mbr_partition_2;
                        MBRDsk.mbr_partition_2 = MBRDsk.mbr_partition_1;
                        MBRDsk.mbr_partition_1.part_status = 'A';
                        if (strLower(Fit) == "bf")
                        {
                            MBRDsk.mbr_partition_1.part_fit = 'B';
                        }
                        else if (strLower(Fit) == "ff")
                        {
                            MBRDsk.mbr_partition_1.part_fit = 'F';
                        }
                        else
                        {
                            MBRDsk.mbr_partition_1.part_fit = 'W';
                        }
                        MBRDsk.mbr_partition_1.part_start = sizeof(MBRDsk);
                        MBRDsk.mbr_partition_1.part_size = Size;
                        strcpy(MBRDsk.mbr_partition_1.part_name, Name.c_str());
                        if (strLower(Type) == "p")
                        {
                            MBRDsk.mbr_partition_1.part_type = 'P';
                        }
                        else
                        {
                            MBRDsk.mbr_partition_1.part_type = 'E';
                            EBR NewEBR;
                            NewEBR.part_status = 'D';
                            NewEBR.part_fit = ' ';
                            NewEBR.part_start = MBRDsk.mbr_partition_1.part_start;
                            NewEBR.part_size = -1;
                            NewEBR.part_next = -1;
                            strcpy(NewEBR.part_name, "");
                            if (MBRDsk.mbr_partition_1.part_size <= sizeof(NewEBR))
                            {
                                Registrado = false;
                                cout << "Error, no hay espacio para un EBR" << endl;
                            }
                            else
                            {
                                file.seekg(MBRDsk.mbr_partition_1.part_start);
                                file.write(reinterpret_cast<char *>(&NewEBR), sizeof(NewEBR));
                            }
                        }
                    }
                    else if (RegistrarEn == 2)
                    {
                        MBRDsk.mbr_partition_4 = MBRDsk.mbr_partition_3;
                        MBRDsk.mbr_partition_3 = MBRDsk.mbr_partition_2;
                        MBRDsk.mbr_partition_2.part_status = 'A';
                        if (strLower(Fit) == "bf")
                        {
                            MBRDsk.mbr_partition_2.part_fit = 'B';
                        }
                        else if (strLower(Fit) == "ff")
                        {
                            MBRDsk.mbr_partition_2.part_fit = 'F';
                        }
                        else
                        {
                            MBRDsk.mbr_partition_2.part_fit = 'W';
                        }
                        MBRDsk.mbr_partition_2.part_start = MBRDsk.mbr_partition_1.part_start + MBRDsk.mbr_partition_1.part_size;
                        MBRDsk.mbr_partition_2.part_size = Size;
                        strcpy(MBRDsk.mbr_partition_2.part_name, Name.c_str());
                        if (strLower(Type) == "p")
                        {
                            MBRDsk.mbr_partition_2.part_type = 'P';
                        }
                        else
                        {
                            MBRDsk.mbr_partition_2.part_type = 'E';
                            EBR NewEBR;
                            NewEBR.part_status = 'D';
                            NewEBR.part_fit = ' ';
                            NewEBR.part_start = MBRDsk.mbr_partition_2.part_start;
                            NewEBR.part_size = -1;
                            NewEBR.part_next = -1;
                            strcpy(NewEBR.part_name, "");
                            if (MBRDsk.mbr_partition_2.part_size <= sizeof(NewEBR))
                            {
                                Registrado = false;
                                cout << "Error, no hay espacio para un EBR" << endl;
                            }
                            else
                            {
                                file.seekg(MBRDsk.mbr_partition_2.part_start);
                                file.write(reinterpret_cast<char *>(&NewEBR), sizeof(NewEBR));
                            }
                        }
                    }
                    else if (RegistrarEn == 3)
                    {
                        MBRDsk.mbr_partition_4 = MBRDsk.mbr_partition_3;
                        MBRDsk.mbr_partition_3.part_status = 'A';
                        if (strLower(Fit) == "bf")
                        {
                            MBRDsk.mbr_partition_3.part_fit = 'B';
                        }
                        else if (strLower(Fit) == "ff")
                        {
                            MBRDsk.mbr_partition_3.part_fit = 'F';
                        }
                        else
                        {
                            MBRDsk.mbr_partition_3.part_fit = 'W';
                        }
                        MBRDsk.mbr_partition_3.part_start = MBRDsk.mbr_partition_2.part_start + MBRDsk.mbr_partition_2.part_size;
                        MBRDsk.mbr_partition_3.part_size = Size;
                        strcpy(MBRDsk.mbr_partition_3.part_name, Name.c_str());
                        if (strLower(Type) == "p")
                        {
                            MBRDsk.mbr_partition_3.part_type = 'P';
                        }
                        else
                        {
                            MBRDsk.mbr_partition_3.part_type = 'E';
                            EBR NewEBR;
                            NewEBR.part_status = 'D';
                            NewEBR.part_fit = ' ';
                            NewEBR.part_start = MBRDsk.mbr_partition_3.part_start;
                            NewEBR.part_size = -1;
                            NewEBR.part_next = -1;
                            strcpy(NewEBR.part_name, "");
                            if (MBRDsk.mbr_partition_3.part_size <= sizeof(NewEBR))
                            {
                                Registrado = false;
                                cout << "Error, no hay espacio para un EBR" << endl;
                            }
                            else
                            {
                                file.seekg(MBRDsk.mbr_partition_3.part_start);
                                file.write(reinterpret_cast<char *>(&NewEBR), sizeof(NewEBR));
                            }
                        }
                    }
                    else if (RegistrarEn == 4)
                    {
                        MBRDsk.mbr_partition_4.part_status = 'A';
                        if (strLower(Fit) == "bf")
                        {
                            MBRDsk.mbr_partition_4.part_fit = 'B';
                        }
                        else if (strLower(Fit) == "ff")
                        {
                            MBRDsk.mbr_partition_4.part_fit = 'F';
                        }
                        else
                        {
                            MBRDsk.mbr_partition_4.part_fit = 'W';
                        }
                        MBRDsk.mbr_partition_4.part_start = MBRDsk.mbr_partition_3.part_start + MBRDsk.mbr_partition_3.part_size;
                        MBRDsk.mbr_partition_4.part_size = Size;
                        strcpy(MBRDsk.mbr_partition_4.part_name, Name.c_str());
                        if (strLower(Type) == "p")
                        {
                            MBRDsk.mbr_partition_4.part_type = 'P';
                        }
                        else
                        {
                            MBRDsk.mbr_partition_4.part_type = 'E';
                            EBR NewEBR;
                            NewEBR.part_status = 'D';
                            NewEBR.part_fit = ' ';
                            NewEBR.part_start = MBRDsk.mbr_partition_4.part_start;
                            NewEBR.part_size = -1;
                            NewEBR.part_next = -1;
                            strcpy(NewEBR.part_name, "");
                            if (MBRDsk.mbr_partition_4.part_size <= sizeof(NewEBR))
                            {
                                Registrado = false;
                                cout << "Error, no hay espacio para un EBR" << endl;
                            }
                            else
                            {
                                file.seekg(MBRDsk.mbr_partition_4.part_start);
                                file.write(reinterpret_cast<char *>(&NewEBR), sizeof(NewEBR));
                            }
                        }
                    }
                    else
                    {
                        Registrado = false;
                        cout << "Error, no hay espacio interno suficiente para crear la partición" << endl;
                    }
                    if (Registrado)
                    {
                        file.seekg(0);
                        file.write(reinterpret_cast<char *>(&MBRDsk), sizeof(MBRDsk));
                        cout << "Partición creada" << endl;
                    }
                }
                else
                {
                    cout << "Error, El disco ya no puede crear mas particiones" << endl;
                }
            }
            else
            {
                cout << "Error, Ya existe una partición tipo Extendida" << endl;
            }
        }
        else
        {
            int Cabeza = -1;
            int PartitionSize;
            if (MBRDsk.mbr_partition_1.part_status == 'A' && MBRDsk.mbr_partition_1.part_type == 'E')
            {
                Cabeza = MBRDsk.mbr_partition_1.part_start;
                PartitionSize = MBRDsk.mbr_partition_1.part_size;
            }
            else if (MBRDsk.mbr_partition_2.part_status == 'A' && MBRDsk.mbr_partition_2.part_type == 'E')
            {
                Cabeza = MBRDsk.mbr_partition_2.part_start;
                PartitionSize = MBRDsk.mbr_partition_2.part_size;
            }
            else if (MBRDsk.mbr_partition_3.part_status == 'A' && MBRDsk.mbr_partition_3.part_type == 'E')
            {
                Cabeza = MBRDsk.mbr_partition_3.part_start;
                PartitionSize = MBRDsk.mbr_partition_3.part_size;
            }
            else if (MBRDsk.mbr_partition_4.part_status == 'A' && MBRDsk.mbr_partition_4.part_type == 'E')
            {
                Cabeza = MBRDsk.mbr_partition_4.part_start;
                PartitionSize = MBRDsk.mbr_partition_4.part_size;
            }
            if (Cabeza != -1)
            {
                EBR EBRAux;
                file.seekg(Cabeza);
                file.read(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                if (EBRAux.part_status == 'D' && EBRAux.part_next == -1)
                {
                    if (Size <= PartitionSize)
                    {
                        EBRAux.part_status = 'A';
                        if (strLower(Fit) == "bf")
                        {
                            EBRAux.part_fit = 'B';
                        }
                        else if (strLower(Fit) == "ff")
                        {
                            EBRAux.part_fit = 'F';
                        }
                        else
                        {
                            EBRAux.part_fit = 'W';
                        }
                        EBRAux.part_start = Cabeza;
                        EBRAux.part_size = Size;
                        EBRAux.part_next = -1;
                        strcpy(EBRAux.part_name, Name.c_str());
                        file.seekg(Cabeza);
                        file.write(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                        cout << "Partición creada" << endl;
                    }
                    else
                    {
                        cout << "Error, no hay espacio suficiete para la partición logica" << endl;
                    }
                }
                else
                {
                    EBR EBRAux;
                    file.seekg(Cabeza);
                    file.read(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                    int Actual = Cabeza;
                    int InsertarEN = -1;
                    if (strLower(Fit) == "bf")
                    {
                        int Menor = PartitionSize;
                        if (EBRAux.part_status == 'D')
                        {
                            if ((EBRAux.part_next - (EBRAux.part_start + EBRAux.part_size)) >= Size)
                            {
                                Menor = EBRAux.part_next - (EBRAux.part_start + EBRAux.part_size);
                                InsertarEN = EBRAux.part_start;
                                Actual = EBRAux.part_next;
                            }
                        }
                        while (Actual != -1)
                        {
                            file.seekg(Actual);
                            file.read(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                            if (EBRAux.part_next != -1)
                            {
                                if ((EBRAux.part_next - (EBRAux.part_start + EBRAux.part_size)) >= Size)
                                {
                                    if (Menor > EBRAux.part_next - (EBRAux.part_start + EBRAux.part_size))
                                    {
                                        Menor = EBRAux.part_next - (EBRAux.part_start + EBRAux.part_size);
                                        InsertarEN = EBRAux.part_start;
                                    }
                                }
                            }
                            else
                            {
                                if (((Cabeza + PartitionSize) - (EBRAux.part_start + EBRAux.part_size)) >= Size)
                                {
                                    if (Menor > (Cabeza + PartitionSize) - (EBRAux.part_start + EBRAux.part_size))
                                    {
                                        InsertarEN = EBRAux.part_start;
                                    }
                                }
                            }
                            Actual = EBRAux.part_next;
                        }
                    }
                    else if (strLower(Fit) == "ff")
                    {
                        bool Encontrado = false;
                        if (EBRAux.part_status == 'D')
                        {
                            if ((EBRAux.part_next - (EBRAux.part_start + EBRAux.part_size)) >= Size)
                            {
                                Encontrado = true;
                                InsertarEN = EBRAux.part_start;
                                Actual = EBRAux.part_next;
                            }
                        }
                        while (Actual != -1 && !Encontrado)
                        {
                            file.seekg(Actual);
                            file.read(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                            if (EBRAux.part_next != -1)
                            {
                                if ((EBRAux.part_next - (EBRAux.part_start + EBRAux.part_size)) >= Size)
                                {
                                    Encontrado = true;
                                    InsertarEN = EBRAux.part_start;
                                }
                            }
                            else
                            {
                                if (((Cabeza + PartitionSize) - (EBRAux.part_start + EBRAux.part_size)) >= Size)
                                {
                                    Encontrado = true;
                                    InsertarEN = EBRAux.part_start;
                                }
                            }
                            Actual = EBRAux.part_next;
                        }
                    }
                    else
                    {
                        int Mayor = 0;
                        if (EBRAux.part_status == 'D')
                        {
                            if ((EBRAux.part_next - (EBRAux.part_start + EBRAux.part_size)) >= Size)
                            {
                                Mayor = EBRAux.part_next - (EBRAux.part_start + EBRAux.part_size);
                                InsertarEN = EBRAux.part_start;
                                Actual = EBRAux.part_next;
                            }
                        }
                        while (Actual != -1)
                        {
                            file.seekg(Actual);
                            file.read(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                            if (EBRAux.part_next != -1)
                            {
                                if ((EBRAux.part_next - (EBRAux.part_start + EBRAux.part_size)) >= Size)
                                {
                                    if (Mayor < EBRAux.part_next - (EBRAux.part_start + EBRAux.part_size))
                                    {
                                        Mayor = EBRAux.part_next - (EBRAux.part_start + EBRAux.part_size);
                                        InsertarEN = EBRAux.part_start;
                                    }
                                }
                            }
                            else
                            {
                                if (((Cabeza + PartitionSize) - (EBRAux.part_start + EBRAux.part_size)) >= Size)
                                {
                                    if (Mayor < (Cabeza + PartitionSize) - (EBRAux.part_start + EBRAux.part_size))
                                    {
                                        InsertarEN = EBRAux.part_start;
                                    }
                                }
                            }
                            Actual = EBRAux.part_next;
                        }
                    }
                    if (InsertarEN != -1)
                    {
                        file.seekg(Cabeza);
                        file.read(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                        if (InsertarEN == Cabeza && EBRAux.part_status == 'D')
                        {
                            EBRAux.part_status = 'A';
                            if (strLower(Fit) == "bf")
                            {
                                EBRAux.part_fit = 'B';
                            }
                            else if (strLower(Fit) == "ff")
                            {
                                EBRAux.part_fit = 'F';
                            }
                            else
                            {
                                EBRAux.part_fit = 'W';
                            }
                            EBRAux.part_size = Size;
                            strcpy(EBRAux.part_name, Name.c_str());
                            file.seekg(Cabeza);
                            file.write(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                            cout << "Partición creada con exito" << endl;
                        }
                        else
                        {
                            file.seekg(InsertarEN);
                            file.read(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                            EBR NewEBR;
                            NewEBR.part_status = 'A';
                            if (strLower(Fit) == "bf")
                            {
                                NewEBR.part_fit = 'B';
                            }
                            else if (strLower(Fit) == "ff")
                            {
                                NewEBR.part_fit = 'F';
                            }
                            else
                            {
                                NewEBR.part_fit = 'W';
                            }
                            NewEBR.part_start = EBRAux.part_start + EBRAux.part_size;
                            NewEBR.part_size = Size;
                            NewEBR.part_next = EBRAux.part_next;
                            EBRAux.part_next = NewEBR.part_start;
                            strcpy(NewEBR.part_name, Name.c_str());
                            file.seekg(InsertarEN);
                            file.write(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                            file.seekg(NewEBR.part_start);
                            file.write(reinterpret_cast<char *>(&NewEBR), sizeof(NewEBR));
                            cout << "Partición creada con exito" << endl;
                        }
                    }
                    else
                    {
                        cout << "Error, no hay espacio interno suficiente para crear la partición" << endl;
                    }
                }
            }
            else
            {
                cout << "Error, no hay partición extendida para crear una partición logica" << endl;
            }
        }
    }
    else
    {
        cout << "Error, no se pudo abrir el archivo" << endl;
    }
    file.close();
}

void EliminarParticion(string Path, string Name)
{
    MBR MBRDsk;
    fstream file(Path, ios::binary | ios::in | ios::out);
    if (file.is_open())
    {
        bool Eliminado = false;
        int Cabeza = -1;
        file.read(reinterpret_cast<char *>(&MBRDsk), sizeof(MBRDsk));
        if (MBRDsk.mbr_partition_1.part_status == 'A')
        {
            string NameA = MBRDsk.mbr_partition_1.part_name;
            if (NameA == Name)
            {
                MBRDsk.mbr_partition_1 = MBRDsk.mbr_partition_2;
                MBRDsk.mbr_partition_2 = MBRDsk.mbr_partition_3;
                MBRDsk.mbr_partition_3 = MBRDsk.mbr_partition_4;
                MBRDsk.mbr_partition_4.part_status = 'D';
                MBRDsk.mbr_partition_4.part_type = ' ';
                MBRDsk.mbr_partition_4.part_fit = ' ';
                MBRDsk.mbr_partition_4.part_start = -1;
                MBRDsk.mbr_partition_4.part_size = -1;
                strcpy(MBRDsk.mbr_partition_4.part_name, "");
                Eliminado = true;
            }
            else
            {
                if (MBRDsk.mbr_partition_1.part_type == 'E')
                {
                    Cabeza = MBRDsk.mbr_partition_1.part_start;
                }
                if (MBRDsk.mbr_partition_2.part_status == 'A')
                {
                    NameA = MBRDsk.mbr_partition_2.part_name;
                    if (NameA == Name)
                    {
                        MBRDsk.mbr_partition_2 = MBRDsk.mbr_partition_3;
                        MBRDsk.mbr_partition_3 = MBRDsk.mbr_partition_4;
                        MBRDsk.mbr_partition_4.part_status = 'D';
                        MBRDsk.mbr_partition_4.part_type = ' ';
                        MBRDsk.mbr_partition_4.part_fit = ' ';
                        MBRDsk.mbr_partition_4.part_start = -1;
                        MBRDsk.mbr_partition_4.part_size = -1;
                        strcpy(MBRDsk.mbr_partition_4.part_name, "");
                        Eliminado = true;
                    }
                    else
                    {
                        if (MBRDsk.mbr_partition_2.part_type == 'E')
                        {
                            Cabeza = MBRDsk.mbr_partition_2.part_start;
                        }
                        if (MBRDsk.mbr_partition_3.part_status == 'A')
                        {
                            NameA = MBRDsk.mbr_partition_3.part_name;
                            if (NameA == Name)
                            {
                                MBRDsk.mbr_partition_3 = MBRDsk.mbr_partition_4;
                                MBRDsk.mbr_partition_4.part_status = 'D';
                                MBRDsk.mbr_partition_4.part_type = ' ';
                                MBRDsk.mbr_partition_4.part_fit = ' ';
                                MBRDsk.mbr_partition_4.part_start = -1;
                                MBRDsk.mbr_partition_4.part_size = -1;
                                strcpy(MBRDsk.mbr_partition_4.part_name, "");
                                Eliminado = true;
                            }
                            else
                            {
                                if (MBRDsk.mbr_partition_3.part_type == 'E')
                                {
                                    Cabeza = MBRDsk.mbr_partition_3.part_start;
                                }
                                if (MBRDsk.mbr_partition_4.part_status == 'A')
                                {
                                    NameA = MBRDsk.mbr_partition_4.part_name;
                                    if (NameA == Name)
                                    {
                                        MBRDsk.mbr_partition_4.part_status = 'D';
                                        MBRDsk.mbr_partition_4.part_type = ' ';
                                        MBRDsk.mbr_partition_4.part_fit = ' ';
                                        MBRDsk.mbr_partition_4.part_start = -1;
                                        MBRDsk.mbr_partition_4.part_size = -1;
                                        strcpy(MBRDsk.mbr_partition_4.part_name, "");
                                        Eliminado = true;
                                    }
                                    else
                                    {
                                        if (MBRDsk.mbr_partition_4.part_type == 'E')
                                        {
                                            Cabeza = MBRDsk.mbr_partition_4.part_start;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (!Eliminado && Cabeza != -1)
        {
            EBR EBRAux;
            file.seekg(Cabeza);
            file.read(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
            string NameA = EBRAux.part_name;
            if (EBRAux.part_status == 'A' && NameA == Name)
            {
                EBRAux.part_status = 'D';
                EBRAux.part_fit = ' ';
                EBRAux.part_size = -1;
                strcpy(EBRAux.part_name, "");
                file.seekg(EBRAux.part_start);
                file.write(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                Eliminado = true;
            }
            while (EBRAux.part_next != -1 && !Eliminado)
            {
                EBR EBRNext;
                file.seekg(EBRAux.part_next);
                file.read(reinterpret_cast<char *>(&EBRNext), sizeof(EBRNext));
                NameA = EBRNext.part_name;
                if (NameA == Name)
                {
                    EBRAux.part_next = EBRNext.part_next;
                    file.seekg(EBRAux.part_start);
                    file.write(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                    Eliminado = true;
                }
            }
        }
        if (Eliminado)
        {
            file.seekg(0);
            file.write(reinterpret_cast<char *>(&MBRDsk), sizeof(MBRDsk));
            cout << "Particion eliminada" << endl;
        }
        else
        {
            cout << "Error, no se encontro la partición" << endl;
        }
    }
    else
    {
        cout << "Error, no se pudo abrir el archivo" << endl;
    }
    file.close();
}

void MountParticion(string Path, string Name)
{
    bool Mount = false;
    for (int i = 0; i < 10; i++)
    {
        if (!ActivePart[i].Active)
        {
            Mount = true;
            MBR MBRDsk;
            ifstream file(Path, ios::binary);
            if (file.is_open())
            {
                MountPart NewMount = BuscarParticion(Path, Name);
                if (NewMount.Active)
                {
                    NewMount.ID = "00" + NewMount.ID + getName(Path);
                    NewMount.Path = Path;
                    strcpy(NewMount.part_name, Name.c_str());
                    ActivePart[i] = NewMount;
                    cout << "Particion Montada" << endl;
                }
                else
                {
                    cout << "Error, no se encontro la particion indicada" << endl;
                }
            }
            else
            {
                cout << "Error, no se pudo abrir el archivo" << endl;
            }
            file.close();
            break;
        }
    }
    if (!Mount)
    {
        cout << "Error, no se pueden montar mas particiones" << endl;
    }
}

void UnmountParticion(string ID)
{
    bool Unmount = false;
    for (int i = 0; i < 10; i++)
    {
        if (!Unmount)
        {
            if (ActivePart[i].Active)
            {
                if (ID == ActivePart[i].ID)
                {
                    Unmount = true;
                    if (i == 9)
                    {
                        ActivePart[i].Active = false;
                    }
                    else
                    {
                        ActivePart[i] = ActivePart[i + 1];
                    }
                }
            }
        }
        else
        {
            if (i == 9)
            {
                ActivePart[i].Active = false;
            }
            else
            {
                ActivePart[i] = ActivePart[i + 1];
            }
        }
    }
    if (!Unmount)
    {
        cout << "Error, no se encontro la particion" << endl;
    }
}

MountPart BuscarParticion(string Path, string Name)
{
    MountPart NewMount;
    NewMount.Active = false;
    MBR MBRDsk;
    fstream file(Path, ios::binary | ios::in | ios::out);
    if (file.is_open())
    {
        int Cabeza = -1;
        file.read(reinterpret_cast<char *>(&MBRDsk), sizeof(MBRDsk));
        if (MBRDsk.mbr_partition_1.part_status == 'A')
        {
            string NameA = MBRDsk.mbr_partition_1.part_name;
            if (NameA == Name)
            {
                NewMount.Active = true;
                strcpy(NewMount.part_name, MBRDsk.mbr_partition_1.part_name);
                NewMount.part_size = MBRDsk.mbr_partition_1.part_size;
                NewMount.part_start = MBRDsk.mbr_partition_1.part_start;
                NewMount.ID = "1";
                return NewMount;
            }
            else
            {
                if (MBRDsk.mbr_partition_1.part_type == 'E')
                {
                    Cabeza = MBRDsk.mbr_partition_1.part_start;
                }
                if (MBRDsk.mbr_partition_2.part_status == 'A')
                {
                    NameA = MBRDsk.mbr_partition_2.part_name;
                    if (NameA == Name)
                    {
                        NewMount.Active = true;
                        strcpy(NewMount.part_name, MBRDsk.mbr_partition_2.part_name);
                        NewMount.part_size = MBRDsk.mbr_partition_2.part_size;
                        NewMount.part_start = MBRDsk.mbr_partition_2.part_start;
                        NewMount.ID = "2";
                        return NewMount;
                    }
                    else
                    {
                        if (MBRDsk.mbr_partition_2.part_type == 'E')
                        {
                            Cabeza = MBRDsk.mbr_partition_2.part_start;
                        }
                        if (MBRDsk.mbr_partition_3.part_status == 'A')
                        {
                            NameA = MBRDsk.mbr_partition_3.part_name;
                            if (NameA == Name)
                            {
                                NewMount.Active = true;
                                strcpy(NewMount.part_name, MBRDsk.mbr_partition_3.part_name);
                                NewMount.part_size = MBRDsk.mbr_partition_3.part_size;
                                NewMount.part_start = MBRDsk.mbr_partition_3.part_start;
                                NewMount.ID = "3";
                                return NewMount;
                            }
                            else
                            {
                                if (MBRDsk.mbr_partition_3.part_type == 'E')
                                {
                                    Cabeza = MBRDsk.mbr_partition_3.part_start;
                                }
                                if (MBRDsk.mbr_partition_4.part_status == 'A')
                                {
                                    NameA = MBRDsk.mbr_partition_4.part_name;
                                    if (NameA == Name)
                                    {
                                        NewMount.Active = true;
                                        strcpy(NewMount.part_name, MBRDsk.mbr_partition_4.part_name);
                                        NewMount.part_size = MBRDsk.mbr_partition_4.part_size;
                                        NewMount.part_start = MBRDsk.mbr_partition_4.part_start;
                                        NewMount.ID = "4";
                                        return NewMount;
                                    }
                                    else
                                    {
                                        if (MBRDsk.mbr_partition_4.part_type == 'E')
                                        {
                                            Cabeza = MBRDsk.mbr_partition_4.part_start;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (Cabeza != -1)
        {
            EBR EBRAux;
            file.seekg(Cabeza);
            file.read(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
            string NameA = EBRAux.part_name;
            int cont = 5;
            if (EBRAux.part_status == 'A' && NameA == Name)
            {
                NewMount.Active = true;
                strcpy(NewMount.part_name, EBRAux.part_name);
                NewMount.part_size = EBRAux.part_size;
                NewMount.part_start = EBRAux.part_start;
                NewMount.ID = to_string(cont);
                return NewMount;
            }
            while (EBRAux.part_next != -1)
            {
                cont++;
                EBR EBRNext;
                file.seekg(EBRAux.part_next);
                file.read(reinterpret_cast<char *>(&EBRNext), sizeof(EBRNext));
                NameA = EBRNext.part_name;
                if (NameA == Name)
                {
                    NewMount.Active = true;
                    strcpy(NewMount.part_name, EBRNext.part_name);
                    NewMount.part_size = EBRNext.part_size;
                    NewMount.part_start = EBRNext.part_start;
                    NewMount.ID = to_string(cont);
                    return NewMount;
                }
            }
        }
        cout << "Error, no se encontro la partición" << endl;
    }
    file.close();
    return NewMount;
}

void LeerScript(string Path)
{
    ifstream ScriptFile(Path);
    if (ScriptFile.is_open())
    {
        string Linea;
        while (getline(ScriptFile, Linea))
        {
            LeerComando(Linea + " ");
        }
    }
    ScriptFile.close();
}

string Valor(string &Linea)
{
    int Divisor;
    string Val = "";
    if (Linea[0] != '"')
    {
        if (Linea.find(' ') < Linea.find('='))
        {
            Val = Linea.substr(0, Linea.find(' '));
            Linea = Linea.substr(Linea.find(' ') + 1, Linea.length());
        }
        else
        {
            Val = Linea.substr(0, Linea.find('='));
            Linea = Linea.substr(Linea.find('=') + 1, Linea.length());
        }
    }
    else
    {
        Linea = Linea.substr(1, Linea.length());
        Val = Linea.substr(0, Linea.find('"'));
        Linea = Linea.substr(Linea.find('"') + 1, Linea.length());
    }
    while (Linea.length() > 0 && Linea[0] == ' ')
    {
        Linea = Linea.substr(1, Linea.length());
    }
    return Val;
}

void CrearPath(string Path)
{
    string SubPath = "";
    while (Path.find("/") != string::npos)
    {
        SubPath += Path.substr(0, Path.find("/") + 1);
        Path = Path.substr(Path.find("/") + 1, Path.length());
        mkdir(SubPath.c_str(), 0777);
    }
}

string getName(string Path)
{
    while (Path.find("/") != string::npos)
    {
        Path = Path.substr(Path.find("/") + 1, Path.length());
    }
    if (Path.find(".") != string::npos)
    {
        Path = Path.substr(0, Path.find("."));
    }
    return Path;
}

void VerDisco(string Path)
{
    MBR MBRDsk;
    ifstream file(Path, ios::binary);
    int Cabeza = -1;
    if (file.is_open())
    {
        string Report = "";
        file.read(reinterpret_cast<char *>(&MBRDsk), sizeof(MBRDsk));
        Report += "MBR - " + to_string(sizeof(MBRDsk)) + "\n";
        Report += "Tamaño - " + to_string(MBRDsk.mbr_tamano) + "\n";
        Report += "Fecha - ";
        Report += ctime(&MBRDsk.mbr_fecha_creacion);
        Report += "Numero - " + to_string(MBRDsk.mbr_dsk_signature) + "\n";
        Report += "Fit - ";
        Report += +MBRDsk.mbr_fit;
        if (MBRDsk.mbr_partition_1.part_status == 'A')
        {
            Report += "\n\tpartition 1.\n\t\tType: ";
            Report += MBRDsk.mbr_partition_1.part_type;
            Report += "\n\t\tFit: ";
            Report += MBRDsk.mbr_partition_1.part_fit;
            Report += "\n\t\tStart: " + to_string(MBRDsk.mbr_partition_1.part_start) + "\n";
            Report += "\t\tSize: " + to_string(MBRDsk.mbr_partition_1.part_size) + "\n\t\tName: ";
            Report += MBRDsk.mbr_partition_1.part_name;
            Report + "\n";
            if (MBRDsk.mbr_partition_1.part_type == 'E')
            {
                Cabeza = MBRDsk.mbr_partition_1.part_start;
            }
            if (MBRDsk.mbr_partition_2.part_status == 'A')
            {
                Report += "\n\tpartition 2.\n\t\tType: ";
                Report += MBRDsk.mbr_partition_2.part_type;
                Report += "\n\t\tFit: ";
                Report += MBRDsk.mbr_partition_2.part_fit;
                Report += "\n\t\tStart: " + to_string(MBRDsk.mbr_partition_2.part_start) + "\n";
                Report += "\t\tSize: " + to_string(MBRDsk.mbr_partition_2.part_size) + "\n\t\tName: ";
                Report += MBRDsk.mbr_partition_2.part_name;
                Report + "\n";
                if (MBRDsk.mbr_partition_2.part_type == 'E')
                {
                    Cabeza = MBRDsk.mbr_partition_2.part_start;
                }
                if (MBRDsk.mbr_partition_3.part_status == 'A')
                {
                    Report += "\n\tpartition 3.\n\t\tType: ";
                    Report += MBRDsk.mbr_partition_3.part_type;
                    Report += "\n\t\tFit: ";
                    Report += MBRDsk.mbr_partition_3.part_fit;
                    Report += "\n\t\tStart: " + to_string(MBRDsk.mbr_partition_3.part_start) + "\n";
                    Report += "\t\tSize: " + to_string(MBRDsk.mbr_partition_3.part_size) + "\n\t\tName: ";
                    Report += MBRDsk.mbr_partition_3.part_name;
                    Report + "\n";
                    if (MBRDsk.mbr_partition_3.part_type == 'E')
                    {
                        Cabeza = MBRDsk.mbr_partition_3.part_start;
                    }
                    if (MBRDsk.mbr_partition_4.part_status == 'A')
                    {
                        Report += "\n\tpartition 4.\n\t\tType: ";
                        Report += MBRDsk.mbr_partition_4.part_type;
                        Report += "\n\t\tFit: ";
                        Report += MBRDsk.mbr_partition_4.part_fit;
                        Report += "\n\t\tStart: " + to_string(MBRDsk.mbr_partition_4.part_start) + "\n";
                        Report += "\t\tSize: " + to_string(MBRDsk.mbr_partition_4.part_size) + "\n\t\tName: ";
                        Report += MBRDsk.mbr_partition_4.part_name;
                        Report + "\n";
                        if (MBRDsk.mbr_partition_4.part_type == 'E')
                        {
                            Cabeza = MBRDsk.mbr_partition_4.part_start;
                        }
                    }
                }
            }
        }
        if (Cabeza != -1)
        {
            int Aux = Cabeza;
            int Cont = 1;
            while (Aux != -1)
            {
                EBR EBRAux;
                file.seekg(Aux);
                file.read(reinterpret_cast<char *>(&EBRAux), sizeof(EBRAux));
                Report += "\n\tpartition logical. " + to_string(Cont) + "\n\t\tEstatus: ";
                Report += EBRAux.part_status;
                Report += "\n\t\tFit: ";
                Report += EBRAux.part_fit;
                Report += "\n\t\tStart: " + to_string(EBRAux.part_start);
                Report += "\n\t\tSize: " + to_string(EBRAux.part_size);
                Report += "\n\t\tNext: " + to_string(EBRAux.part_next) + "\n\t\tName: ";
                Report += EBRAux.part_name;
                Report + "\n";
                Aux = EBRAux.part_next;
                Cont++;
            }
        }
        cout << Report << endl;
    }
    else
    {
        cout << "Error, no se pudo abrir el archivo" << endl;
    }
    file.close();
}

void VerMounts()
{
    for (int i = 0; i < 10; i++)
    {
        if (ActivePart[i].Active)
        {
            cout << "Particion No. " << (i + 1) << endl;
            cout << "\tID: " << ActivePart[i].ID << endl;
            cout << "\tPath: " << ActivePart[i].Path << endl;
            cout << "\tName: " << ActivePart[i].part_name << endl;
            cout << "\tSize: " << ActivePart[i].part_size << endl;
            cout << "\tStart: " << ActivePart[i].part_start << endl;
        }
        else
        {
            break;
        }
    }
}
