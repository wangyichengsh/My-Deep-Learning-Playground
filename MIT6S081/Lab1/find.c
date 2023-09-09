#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  strcpy(buf, p);
  return buf;
}

int
whether_contain(char *path, char *name){
  uint name_len = strlen(name);
  for(char *p = path;p<=path+strlen(path);p++){
    if(p[0]!=name[0])
      continue;
    char tmp[50];
    memcpy(tmp, p, name_len);
    tmp[name_len] = 0;
    if(strcmp(tmp, name)==0)
      return 0;
  }
  return 1;
}

void
find(char *path, char *name, int flag)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if(whether_contain(path, name)==0)
      printf("%s\n", path);
    break;

  case T_DIR:
    if(flag==1 && (strcmp(fmtname(path),".")==0||strcmp(fmtname(path),"..")==0)){
      break;
    }  
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      uint name_len = strlen(de.name);
      memmove(p, de.name, name_len);
      p[name_len] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      find(buf, name, 1);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "Usage:find {path} {name}\n");
    exit(0);
  }else
    if(strcmp(argv[1],".")||strcmp(argv[1],".."))
      find(argv[1], argv[2], 0);
    else
      find(argv[1], argv[2], 1);
  exit(0);
}
