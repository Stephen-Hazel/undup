// undup.cpp

#include "undup.h"

typedef struct {TStr nm;  long sz;} FDef;

TStr   Top;
FDef   F   [1024*1024];
ulong NF;
char   Buf [8*1024*1024],
       Bf2 [8*1024*1024];


bool DoDir (void *ptr, char dfx, char *fn)
// find any files and put em in F[]
{ File f;
   (void)ptr;
//DBG("DoDir dfx=`c fn='`s'", dfx, fn);
   if ((dfx == 'f') && (NF < BITS (F)))
      {StrCp (F [NF].nm, fn);   F [NF++].sz = f.Size (fn);}
   return false;
}


int SizCmp (void *p1, void *p2)   // by fn's size;  fn
{ FDef *f1 = (FDef *)p1, *f2 = (FDef *)p2;
  int i;
   if ((i =      f1->sz - f2->sz))  return i;
   return StrCm (f1->nm,  f2->nm);
}


char *DoRec (char *buf, uword len, ulong pos, void *ptr)
// parse a record of the file of fns that DON'T GET KILLED
{ TStr fn;
  File f;
   (void)len;   (void)pos;   (void)ptr;
   if (*buf == '\0')              return NULL;
   if (MemCm (buf, CC("x "), 2))  return NULL;
   StrFmt (fn, "`s/`s", Top, & buf [2]);   f.Kill (fn);   return NULL;
}


void UnDup::Go ()
{ File  f, f2;
  TStr  fn, tfn, ufn;
  PStr  ps;
  ulong i, i2, j, k, m, lnD, ngot;
  long  sz;
  char *p;
   StrCp (Top, UnQS (ui->ledDir->text ()));
DBG("UnDup dir=`s", Top);
   NF = 0;
   f.DoDir (Top, this, DoDir);
DBG ("`d files", NF);

   Sort (F, NF, sizeof (F [0]), SizCmp);

// UnDup
   StrFmt (fn, "`s.txt", Top);
   if (! f.Open (fn, "w"))  {DBG (fn, "can't write file :(");   return;}

   lnD = StrLn (Top) + 1;              // offset past top dir
   i = 0;
DBG ("UnDup - Finding exact matches for size matches...");
   while (i < NF) {
      i2 = i;   sz = F [i].sz;         // find size group range
      while ((i2+1 < NF) && (F [i2+1].sz == sz))  i2++;
      if (i == i2)  F [i++].sz = 1;    // flag NON killable file fer later
      else {                           // now do binary cmp on all in size
DBG ("`d-`d of `d size=`d", i+1, i2+1, NF, sz);
         for (j = i;  j < i2;  j++)  if (F [j].sz) {  // sz 0 if already
DBG ("   `d to `d", j+1, i2+1);
            f2.Load (F [j].nm, Buf, sizeof (Buf));    // in therez
            ngot = 0;
            for (k = j+1;  k <= i2;  k++)  if (F [k].sz) {
               f2.Load (F [k].nm, Bf2, sizeof (Bf2));
               if (! MemCm (Buf, Bf2, sz, 'x')) {
               // doin' 1st one too?  or just 3rd n on match?
                  if (! ngot) {
//DBG ("   j=`d `s", j, F [j].nm);
                     StrCp (F [ngot++].nm, & F [j].nm [lnD]);
                     F [j].sz = 0;     // candidates for del'n
                  }
//DBG ("   k=`d `s", k, F [k].nm);
                  StrCp (F [ngot++].nm, & F [k].nm [lnD]);
                  F [k].sz = 0;
               }
            }
            if (ngot)  {
               for (m = 0, k = 1;  k < ngot;  k++) {  // find longest fn
                  FnName (ufn, F [k].nm);   FnName (tfn, F [m].nm);
                  if (StrLn (ufn) > StrLn (tfn))  m = k;
                  else if ((StrLn (ufn) == StrLn (tfn)) &&
                           (StrLn (F [k].nm) > StrLn (F [m].nm)))  m = k;
               }
               f.Put (StrFmt (ps, " `s\n", F [m].nm));
               for (k = 0;  k < ngot;  k++)  if (k != m)
                  f.Put (StrFmt (ps, "x `s\n", F [k].nm));
               f.Put (CC("\n"));
//DBG("   ");
            }
         }
         i = i2 + 1;
      }
   }
   f.Shut ();
   system (StrFmt (tfn, "`s '`s'",
      getenv ("VISUAL") ? getenv ("VISUAL") : "/opt/app/ned", fn));
   if (YNo ("Done re-x'ing lines of files to delete?", fn))
      if ((p = f.DoText (fn, NULL, DoRec)))
         DBG ("error readin your .txt:(", p);

   f.Kill (fn);                        // cleanup
   qApp->quit ();
}


void UnDup::Dir ()
{ TStr s;
   StrCp (s, UnQS (ui->ledDir->text ()));
   if (AskDir (this, s, "Pick a top directory"))  ui->ledDir->setText (s);
}


//______________________________________________________________________________
void UnDup::Open ()
{ QFont f ("Noto Sans Mono", 12);      // calc our window size from our font
  TStr  d;
  int   argc = qApp->arguments ().count ();
   QApplication::setFont (f);
   WinLoad (this, "StephenHazel", "UnDup");
   setWindowTitle ("UnDup");

   if (argc >= 2)  StrCp (d, CC(UnQS (qApp->arguments ().at (1))));
   else if (getcwd (d, sizeof (TStr)) == nullptr)  *d = '\0';
   connect (ui->btnDir, SIGNAL (clicked ()), this, SLOT (Dir ()));
   connect (ui->btnGo,  SIGNAL (clicked ()), this, SLOT (Go  ()));

   if (*d)  ui->ledDir->setText (d);
}


void UnDup::Shut ()   {WinSave (this, "StephenHazel", "UnDup");}


int main (int argc, char *argv [])
{ QApplication a (argc, argv);
  UnDup w;
  int r;
   w.Open ();   r = a.exec ();   w.Shut ();   return r;
}
