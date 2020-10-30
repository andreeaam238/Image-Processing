//Blaga Ana-Maria-Andreea, 314

#include "bmp_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 30

// Structura pentru un element din matricea de pixeli in care este pastrata
// imaginea.

struct pixel {
  unsigned char blue, green, red;
};

// Strctura pentru coada folosita la task-ul de clustering pentru a pastra
// indicii de linie si de coloana ai pixelilor din aceeasi zona.

struct elem {
  int line, col;
};

// Functie pentru alocarea de memorie pentru o matrice de pixeli.

struct pixel **alloc_bitmap_matrix(int no_lines, int no_columns) {
  struct pixel **matrix_pixel = calloc(no_lines, sizeof(struct pixel *));
  int i;

  if (!matrix_pixel) {
    return NULL;
  } else {
    for (i = 0; i < no_lines; i++) {
      matrix_pixel[i] = calloc(no_columns, sizeof(struct pixel));
      
      if (!matrix_pixel[i]) {
        return NULL;
      }
    }
  }
  return matrix_pixel;
}

// Functie pentru eliberarea memoriei alocate pentru o matrice de pixeli.

void free_bitmap_matrix(struct pixel **matrix_pixel, int no_lines) {
  int i, j;
  for (i = 0; i < no_lines; i++) {
    free(matrix_pixel[i]);
  }
  free(matrix_pixel);
}

// Functie pentru citirea file header-ului si a info header-ului unei imagini
// bitmap, cat si a matricei de pixeli corespunzatoare imaginii
// respective(folosind functia alloc_bitmap_matrix) dintr-un fisier
// binar(input_file) ignorand padding-ul(folosind functia fseek).

struct pixel **load(FILE *input_file, bmp_infoheader *info_header,
                    bmp_fileheader *file_header) {
  int i, j, padding = 0;
  
  fread(file_header, sizeof(bmp_fileheader), 1, input_file);
  fread(info_header, sizeof(bmp_infoheader), 1, input_file);
  
  padding = info_header->width % 4;
  
  struct pixel **matrix_pixel;
  matrix_pixel = alloc_bitmap_matrix(info_header->height, info_header->width);
  
  if (matrix_pixel) {
    for (i = info_header->height - 1; i >= 0; i--) {
      if (matrix_pixel[i]) {
        for (j = 0; j < info_header->width; j++) {
          fread(&matrix_pixel[i][j], sizeof(struct pixel), 1, input_file);
        }
        
        if (padding != 0) {
          fseek(input_file, padding, SEEK_CUR);
        }
      }
    }
  }
  return matrix_pixel;
}

// Functie pentru scrierea intr-un fisier binar(output_file) a file header-ului
// si a info-headerului unei imagini bitmap, cat si a matricei de pixeli
// corespunzatoare imaginii respective. Padding-ul este adaugat prin scrierea
// repetata a caracterului 0 de un numar de ori egal cu restul impartirii la 4 a
// latimii imaginii.

void print(FILE *output_file, bmp_infoheader *info_header,
           bmp_fileheader *file_header, struct pixel **file) {
  int i, j, k, padding = 0;
  unsigned char x = 0;

  if ((info_header->width * 3) % 4 != 0)
    padding = info_header->width % 4;
  
  fwrite(file_header, sizeof(bmp_fileheader), 1, output_file);
  fwrite(info_header, sizeof(bmp_infoheader), 1, output_file);
  
  for (i = info_header->height - 1; i >= 0; i--) {
    for (j = 0; j < info_header->width; j++) {
      fwrite(&file[i][j], sizeof(struct pixel), 1, output_file);
    }

    if (padding != 0) {
      for (k = 0; k < padding; k++) {
        fwrite(&x, sizeof(char), 1, output_file);
      }
    }
  }
}

// Functie pentru transformarea unei imagini color in una alb-negru facand suma
// pe componenetele red, green si blue a fiecarui pixel si impartind-o la
// 3(media aritmetica).

struct pixel **black_and_white(bmp_infoheader *info_header,
                               struct pixel **file) {
  unsigned int aux;
  int i, j;
  
  struct pixel **matrix_pixel;
  matrix_pixel = alloc_bitmap_matrix(info_header->height, info_header->width);
  
  if (matrix_pixel) {
    for (i = 0; i < info_header->height; i++) {
      if (matrix_pixel[i]) {
        for (j = 0; j < info_header->width; j++) {
          aux = file[i][j].blue + file[i][j].green + file[i][j].red;
          aux = aux / 3;
          matrix_pixel[i][j].blue = matrix_pixel[i][j].green =
              matrix_pixel[i][j].red = aux;
        }
      }
    }
    return matrix_pixel;
  }
  return NULL;
}

// Functie pentru determinarea valorii maxime dintre doua numere.

int maximum(int a, int b) {
  if (a > b) {
    return a;
  }
  return b;
}

// Functie pentru determinarea valorii minime dintre doua numere.

int minimum(int a, int b) {
  if (a > b) {
    return b;
  }
  return a;
}

// Functie pentru a transforma a imagine bitmap "nepatratica" in una "patratica"
// adaugand pe marginea acesteia chenar alb. Se creeaza o noua imagine complet
// alba si se copiaza peste aceasta imaginea dorita la anumite coordonate.

struct pixel **no_crop(bmp_infoheader *info_header, struct pixel **file) {
  int i, k, j = 0, dif = abs(info_header->width - info_header->height), max,
            nr = 0;
  max = maximum(info_header->width, info_header->height);
  struct pixel **new_image;
  new_image = alloc_bitmap_matrix(max, max);
  
  if (new_image) {
    for (i = 0; i < max; i++) {
      if (new_image[i]) {
        for (j = 0; j < max; j++) {
          new_image[i][j].blue = new_image[i][j].green = new_image[i][j].red =
              255;
        }
      }
    }
    
    if (dif % 2 == 1) {
      nr = 0;
    }
    
    if (max == info_header->width) {
      j = 0;
      for (i = (max - info_header->height) / 2 + nr;
           i < (max - info_header->height) / 2 + info_header->height + nr;
           i++) {
        for (k = 0; k < max; k++)
          new_image[i][k] = file[j][k];
        j++;
      }
    } else {
      for (i = 0; i < max; i++) {
        k = (max - info_header->width) / 2 + nr;
        for (j = 0; j < info_header->width; j++) {
          new_image[i][k] = file[i][j];
          k++;
        }
      }
    }
    
    info_header->width = max;
    info_header->height = max;
    return new_image;
  }
  return NULL;
}

// Functie pentru a aplica unei imagini bitmap un filtru de forma matriceala,
// matrice patratica. Astfel fiecare element din matricea de pixeli se obtine
// facand suma produselor dintre elementul corespunzator din jurul sau si
// elementul corespunzator din matricea de pixeli(matrice de nxn,n
// impar=>fiecare element devine suma a n^2 produse). In cazul in care elementul
// nu are unul/mai multi vecini produsul respectiv va fi inlocuit cu 0.

struct pixel **convulational_layer(FILE *input_file,
                                   bmp_infoheader *info_header,
                                   struct pixel **file) {
  struct pixel **matrix_pixel =
      alloc_bitmap_matrix(info_header->height, info_header->width);
  int i, j, no, lim, k, l, m, n, r, g, b;
  fscanf(input_file, "%d", &no);
  int **filter = calloc(no, sizeof(int *));
  lim = (no - 1) / 2;
  
  if (filter) {
    for (i = 0; i < no; i++) {
      filter[i] = calloc(no, sizeof(int));
      for (j = 0; j < no; j++) {
        fscanf(input_file, "%d", &filter[i][j]);
      }
    }
  }

  if (filter && matrix_pixel) {
    for (i = info_header->height - 1; i >= 0; i--) {
      for (j = 0; j < info_header->width; j++) {
        r = g = b = 0;
        for (m = 0; m < no; m++) {
          for (n = 0; n < no; n++) {
            k = i + (lim - (no - 1 - m));
            l = j + (lim - (no - 1 - n));
            if (k >= 0 && k < info_header->height && l >= 0 &&
                l < info_header->width) {
              r += file[k][l].red * filter[m][n];
              g += file[k][l].green * filter[m][n];
              b += file[k][l].blue * filter[m][n];
            }
          }
        }
        
        if (r < 0) {
          r = 0;
        } else if (r > 255) {
          r = 255;
        }
        if (g < 0) {
          g = 0;
        } else if (g > 255) {
          g = 255;
        }
        if (b < 0) {
          b = 0;
        } else if (b > 255) {
          b = 255;
        }
        
        matrix_pixel[i][j].red = r;
        matrix_pixel[i][j].green = g;
        matrix_pixel[i][j].blue = b;
      }
    }
    
    for (i = 0; i < no; i++) {
      free(filter[i]);
    }
    
    free(filter);
    return matrix_pixel;
  }
  return NULL;
}

// Functie pentru a aplica unei imagini bitmap un filtru de min/max pooling.
// Astfel fiecare element al matricei de pixeli devine egal cu valoarea
// maxima/minima a unui pixel din jurul sau. De exemplu daca avem un filtru de
// pooling egal cu n, fiecare pixel va lua valoarea maxima/minima(comparandu-l
// cu fiecare pixel dintr-o matrice nxn determinata de pixelii din jurul sau,
// pixelul dorit pentru a i se aplica aceasta operatie fiind centrat in aceasta
// matrice). In cazul in care nu are unul/mai multi vecinii acestia vor fi
// egalati cu 0.

struct pixel **pooling(FILE *input_file, bmp_infoheader *info_header,
                       struct pixel **file) {
  struct pixel **matrix_pixel =
      alloc_bitmap_matrix(info_header->height, info_header->width);
  int i, j, no, lim, k, l, m, n, r, g, b;
  char c;
  fscanf(input_file, "%c", &c);
  fscanf(input_file, "%d", &no);
  lim = (no - 1) / 2;
  
  if (matrix_pixel) {
    if (c == 'M') {
      for (i = 0; i < info_header->height; i++) {
        for (j = 0; j < info_header->width; j++) {
          r = g = b = 0;
          
          for (m = 0; m < no; m++) {
            for (n = 0; n < no; n++) {
              k = i + (lim - (no - 1 - m));
              l = j + (lim - (no - 1 - n));
              
              if (k >= 0 && k < info_header->height && l >= 0 &&
                  l < info_header->width) {
                r = maximum(file[k][l].red, r);
                g = maximum(file[k][l].green, g);
                b = maximum(file[k][l].blue, b);
              }
            }
          }
          matrix_pixel[i][j].red = r;
          matrix_pixel[i][j].green = g;
          matrix_pixel[i][j].blue = b;
        }
      }
    } else {
      for (i = 0; i < info_header->height; i++) {
        for (j = 0; j < info_header->width; j++) {
          r = g = b = 255;
          for (m = 0; m < no; m++) {
            for (n = 0; n < no; n++) {
              k = i + (lim - (no - 1 - m));
              l = j + (lim - (no - 1 - n));
              
              if (k >= 0 && k < info_header->height && l >= 0 &&
                  l < info_header->width) {
                r = minimum(file[k][l].red, r);
                g = minimum(file[k][l].green, g);
                b = minimum(file[k][l].blue, b);
              } else {
                r = 0;
                g = 0;
                b = 0;
              }
            }
          }
          
          matrix_pixel[i][j].red = r;
          matrix_pixel[i][j].green = g;
          matrix_pixel[i][j].blue = b;
        }
      }
    }
    return matrix_pixel;
  }
  return NULL;
}

// Functie care determinata daca un element se afla intr-o matrice de pixeli si
// daca acesta nu a fost inclus deja intr-o alta zona in cadrul task-ului de
// clustering.

int pixel_is_free_and_in_matrix(int i, int j, int m, int n, int **matrix) {
  if (i < 0 || j < 0 || i >= m || j >= n)
    return 0;
  if (matrix[i][j] != 0)
    return 0;
  return 1;
}

// Functie care returneaza valoarea in modul a diferentei a doua numere.

int modul(int a, int b) {
  if ((a - b) > 0)
    return a - b;
  return b - a;
}

// Functie pentru a determina zonele in care va fi impartita imaginea bitmap in
// cadrul task-ului de clustering si calculeaza valoarea pe care pixelii din
// aceasta zona o vor lua, apoi le-o atribuie. Pentru a tine evidenta pixelilor
// este folosit un vector de tip "struct elem" cu doua componente, respectiv
// indicele de linie al fiecarui pixel si cel de coloana. Valoarea componentelor
// unui pixel este determinata facand suma pe componete a fiecarui pixel din
// acea zona si apoi impartind aceste sume la numarul de pixeli din zona.

void determinare_zona(int i, int j, int ref_r, int ref_g, int ref_b,
                      int max_dist, bmp_infoheader *info_header,
                      struct pixel **file, int **matrix,
                      struct pixel **matrix_pixel) {
  int *d_linie = calloc(4, sizeof(int)), *d_coloana = calloc(4, sizeof(int));
  d_linie[0] = d_coloana[3] = -1;
  d_linie[1] = d_coloana[0] = d_coloana[2] = d_linie[3] = 0;
  d_linie[2] = d_coloana[1] = 1;
  
  int new_i, new_j, k, dist = 0, p = 0, r = 0, g = 0, b = 0, nr = 1;
  struct elem *queue =
      calloc(info_header->width * info_header->width, sizeof(struct elem));
  
  if (queue && d_coloana && d_linie) {
    queue[p].line = i;
    queue[p].col = j;
    
    for (p = 0; p < nr; p++) {
      r += file[queue[p].line][queue[p].col].red;
      g += file[queue[p].line][queue[p].col].green;
      b += file[queue[p].line][queue[p].col].blue;
      
      for (k = 0; k < 4; k++) {
        new_i = queue[p].line + d_linie[k];
        new_j = queue[p].col + d_coloana[k];
        
        if (pixel_is_free_and_in_matrix(new_i, new_j, info_header->height, info_header->width, matrix)) {
          if ((modul(file[new_i][new_j].red, ref_r) +
               modul((int)file[new_i][new_j].blue, ref_b) +
               modul(file[new_i][new_j].green, ref_g)) <= max_dist) {
            matrix[new_i][new_j] = -1;
            queue[nr].line = new_i;
            queue[nr].col = new_j;
            nr++;
          }
        }
      }
    }
    
    r = r / nr;
    g = g / nr;
    b = b / nr;
    
    for (p = 0; p < nr; p++) {
      matrix_pixel[queue[p].line][queue[p].col].red = r;
      matrix_pixel[queue[p].line][queue[p].col].green = g;
      matrix_pixel[queue[p].line][queue[p].col].blue = b;
    }
    
    free(queue);
    free(d_linie);
    free(d_coloana);
  }
}

// Functie pentru parcurgerea fiecarui pixel si in cazul in care acesta este
// "ok"(se afla in matrice si nu a fost inclus deja intr-o alta zona <=>
// matrix[i][j]==0) se incepe determinarea unei noi zone de pixeli care vor fi
// "clusterizati".

struct pixel **clustering(FILE *input_file, bmp_infoheader *info_header,
                          struct pixel **file) {
  int i, j, no, **matrix = calloc(info_header->height, sizeof(int *));
  struct pixel **matrix_pixel;
  
  matrix_pixel = alloc_bitmap_matrix(info_header->height, info_header->width);
  fscanf(input_file, "%d", &no);
  
  for (i = 0; i < info_header->height; i++) {
    matrix[i] = calloc(info_header->width, sizeof(int));
  }
  
  if (matrix && matrix_pixel) {
    for (i = 0; i < info_header->height; i++) {
      for (j = 0; j < info_header->width; j++) {
        if (matrix[i][j] == 0) {
          matrix[i][j] = -1;
          
          int ref_r = file[i][j].red;
          int ref_g = file[i][j].green;
          int ref_b = file[i][j].blue;
          
          determinare_zona(i, j, ref_r, ref_g, ref_b, no, info_header, file,
                           matrix, matrix_pixel);
        }
      }
    }
    
    for (i = 0; i < info_header->height; i++) {
      free(matrix[i]);
    }
    
    free(matrix);
    return matrix_pixel;
  }
  return NULL;
}

// Main-ul programului.

int main() {

  // Declararea campurilor info_header si file_header ale imaginii bitmap.

  bmp_infoheader info_header;
  bmp_fileheader file_header;

  // Declararea matricei in care va fi citita imaginea initiala si cea finala.

  struct pixel **bmp_file, **aux;

  // Declararea unor siruri de caractere pentru citire si afisare la diferite
  // adrese.

  char *address = calloc(MAX_LENGTH, sizeof(char)),
       *new_address = calloc(MAX_LENGTH, sizeof(char)),
       *filter = calloc(MAX_LENGTH, sizeof(char));

  // Este deschis fisierul din care va fi citita imaginea bitmap si alte
  // elemente necesare celorlalte task-uri.

  FILE *fin = fopen("input.txt", "rt");

  // Verific daca s-a alocat memoria pentru adrese si daca s-a deschis fisierul
  // din care voi citi elementele componente ale imaginii bitmap.

  if (address && fin && new_address && filter) {

    // Citesc adresa la care se afla imaginea bitmap initiala.

    fscanf(fin, "%s", address);

    // Deschid fisierul in care se afla imaginea bitmap initiala.

    FILE *image_ptr = fopen(address, "rb");

    // Daca a fost deschis incep procesul de prelucrare al imaginii bitmap.

    if (image_ptr) {

      // Citesc file header-ul imaginii, info header-ul imaginii si maricea de
      // pixeli, apoi inchid fisierul.

      bmp_file = load(image_ptr, &info_header, &file_header);
      fclose(image_ptr);

      // Rezolvarea task-ului 1:Black&White. Realizez operatia dorita pe
      // imaginea initiala, creez adresa dorita pentru noua imagine,deschid
      // fisierul, verific daca s-a deschis fisierul, o afisez, inchid fisierul,
      // eliberez memoria.

      aux = black_and_white(&info_header, bmp_file);
      address[strlen(address) - 4] = address[strlen(address)];
      strcpy(new_address, address);
      strcat(new_address, "_black_white.bmp");
      FILE *out = fopen(new_address, "wb");
      if (out) {
        print(out, &info_header, &file_header, aux);
        fclose(out);
        free_bitmap_matrix(aux, info_header.height);
      }

      // Rezolvarea task-ului 2:No-crop. Realizez operatia dorita pe
      // imaginea initiala, creez adresa dorita pentru noua imagine,deschid
      // fisierul, verific daca s-a deschis fisierul, o afisez, inchid fisierul,
      // eliberez memoria.

      strcpy(new_address, address);
      strcat(new_address, "_nocrop.bmp");
      out = fopen(new_address, "wb");
      bmp_infoheader new_info = info_header;
      bmp_fileheader new_file = file_header;
      if (new_info.height != new_info.width && out) {
        aux = no_crop(&new_info, bmp_file);
        print(out, &new_info, &file_header, aux);
        fclose(out);
        free_bitmap_matrix(aux, new_info.height);
      } else if (out) {
        print(out, &new_info, &file_header, bmp_file);
        fclose(out);
      }

      // Rezolvarea task-ului 3:Convolutional Layers. Realizez operatia dorita
      // pe imaginea initiala, creez adresa dorita pentru noua imagine,deschid
      // fisierul, verific daca s-a deschis fisierul, o afisez, inchid fisierul,
      // eliberez memoria.

      fscanf(fin, "%s", filter);
      FILE *read = fopen(filter, "rt");
      strcpy(new_address, address);
      strcat(new_address, "_filter.bmp");
      out = fopen(new_address, "wb");
      if (read) {
        aux = convulational_layer(read, &info_header, bmp_file);
        fclose(read);
        if (out) {
          print(out, &info_header, &file_header, aux);
          fclose(out);
          free_bitmap_matrix(aux, info_header.height);
        }
      }

      // Rezolvarea task-ului 4:Min/max Pooling Layer. Realizez operatia dorita
      // pe imaginea initiala, creez adresa dorita pentru noua imagine,deschid
      // fisierul, verific daca s-a deschis fisierul, o afisez, inchid fisierul,
      // eliberez memoria.

      fscanf(fin, "%s", filter);
      read = fopen(filter, "rt");
      strcpy(new_address, address);
      strcat(new_address, "_pooling.bmp");
      out = fopen(new_address, "wb");
      if (read) {
        aux = pooling(read, &info_header, bmp_file);
        fclose(read);
        if (out) {
          print(out, &info_header, &file_header, aux);
          fclose(out);
          free_bitmap_matrix(aux, info_header.height);
        }
      }

      // Rezolvarea task-ului 5:Clustering. Realizez operatia dorita pe
      // imaginea initiala, creez adresa dorita pentru noua imagine,deschid
      // fisierul, verific daca s-a deschis fisierul, o afisez, inchid fisierul,
      // eliberez memoria.

      fscanf(fin, "%s", filter);
      read = fopen(filter, "rt");
      strcpy(new_address, address);
      strcat(new_address, "_clustered.bmp");
      out = fopen(new_address, "wb");
      if (read && out) {
        aux = clustering(read, &info_header, bmp_file);
        print(out, &info_header, &file_header, aux);
        fclose(out);
        fclose(read);
        free_bitmap_matrix(aux, info_header.height);
      }
    }

    // Eliberez memoria si inchid fisierele deschise.

    free_bitmap_matrix(bmp_file, info_header.height);
    fclose(fin);
    free(new_address);
    free(filter);
    free(address);
  }
  return 0;
}