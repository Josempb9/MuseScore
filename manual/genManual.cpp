//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2002-2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#include <stdio.h>

QString srcPath;

//---------------------------------------------------------
//   Prop
//---------------------------------------------------------

struct Prop {
      QString name;
      QString type;
      QString description;
      };

//---------------------------------------------------------
//   Proc
//---------------------------------------------------------

struct Proc {
      QString name;
      QString type;
      QStringList description;
      };

//---------------------------------------------------------
//   Class
//---------------------------------------------------------

struct Class {
      QString name;
      QString parent;

      QList<Prop> props;
      QList<Proc> procs;

      bool operator<(const Class& c) const {
            return name < c.name;
            }
      };


static QList<Class> classes;

//---------------------------------------------------------
//   addHeader
//---------------------------------------------------------

static void addHeader(QString& out)
      {
      out += "<!DOCTYPE html>\n"
             "<html>\n"
             "<head>\n"
             "   <meta charset=\"utf-8\">\n"
             "   <link rel=\"stylesheet\" type=\"text/css\" href=\"manual.css\"/>\n"
             "   </head>\n"
             "<body>\n";
      }

//---------------------------------------------------------
//   addFooter
//---------------------------------------------------------

static void addFooter(QString& out)
      {
      out += "</body>\n"
             "</html>\n";
      }

//---------------------------------------------------------
//   parseClass
//---------------------------------------------------------

static void parseClass(const QString& name, const QString& in)
      {
      Class cl;
      cl.name = name;

      QStringList sl = in.split("\n");
      QStringList methodDescription;

      QRegExp re("@P ([^\\s]+)\\s+([^\\s]+)(.*)");

      // matches Q_INVOKABLE void mops(int a);   // comment
      QRegExp re1("Q_INVOKABLE +([^ ]+) +([^;]+); */*(.*)");
      QRegExp re2("Q_INVOKABLE +([^ ]+) +([^\\{]+)\\{");
      QRegExp re3("Q_INVOKABLE +([^ ]+) +(\\w+\\([^\\)]*\\))\\s+const\\s*([^\\{]*)\\{");
      QRegExp reD("//@ (.*)");
      QRegExp re4("class +(\\w+) *: *public +(\\w+) *\\{");

      if (!re1.isValid() || !re2.isValid() || !re3.isValid())
            abort();

      foreach(const QString& s, sl) {
            if (re.indexIn(s, 0) != -1) {
                  Prop p;
                  p.name        = re.cap(1);
                  p.type        = re.cap(2);
                  p.description = re.cap(3);
                  cl.props.append(p);
                  }
            else if (re2.indexIn(s, 0) != -1) {
                  Proc p;
                  p.type        = re2.cap(1);
                  p.name        = re2.cap(2);
                  p.description = methodDescription;
                  methodDescription.clear();
                  cl.procs.append(p);
                  }
            else if (re1.indexIn(s, 0) != -1) {
                  Proc p;
                  p.type        = re1.cap(1);
                  p.name        = re1.cap(2);
                  p.description = methodDescription;
                  methodDescription.clear();
                  cl.procs.append(p);
                  }
            else if (re3.indexIn(s, 0) != -1) {
                  Proc p;
                  p.type        = re3.cap(1);
                  p.name        = re3.cap(2);
                  p.description = methodDescription;
                  methodDescription.clear();
                  cl.procs.append(p);
                  }
            else if (reD.indexIn(s, 0) != -1) {
                  printf("description <%s>\n", qPrintable(reD.cap(1)));
                  methodDescription.append(reD.cap(1));
                  }
            else if (re4.indexIn(s, 0) != -1) {
                  QString parent = re4.cap(2).simplified();
                  if (name == re4.cap(1).simplified() && parent != "QObject") {
                        cl.parent = parent;
                        }
                  else
                        printf("?<%s> derived from <%s>\n", qPrintable(name), qPrintable(parent));

                  }
            }
      classes.append(cl);
      }

//---------------------------------------------------------
//   scanFile
//---------------------------------------------------------

static void scanFile(const QString& in)
      {
      QList<Prop> props;
      QList<Proc> procs;

      QRegExp re("@@ ([^\\n]*)");
      int gpos = 0;
      QString className;
      for (;;) {
            int rv = re.indexIn(in, gpos);
            if (rv == -1) {
                  parseClass(className, in.mid(gpos, in.size() - gpos));
                  break;
                  }
            int next = rv + re.matchedLength();
            if (gpos)
                  parseClass(className, in.mid(gpos, next-gpos));

            className = re.cap(1).simplified();
            gpos = next;
            }
      }

//---------------------------------------------------------
//   writeOutput
//---------------------------------------------------------

static void writeOutput()
      {
      foreach(const Class& cl, classes) {
            QString out;
            addHeader(out);
            out += QString("<h3 class=\"object\">%1</h3>\n").arg(cl.name);

            if (!cl.parent.isEmpty()) {
                  QString path = cl.parent.toLower();
                  out += QString("inherits <a href=\"%1.html\">%2</a><br/><br/>\n").arg(path).arg(cl.parent);
                  }

            if (!cl.procs.isEmpty()) {
                  out += "<h4 class=\"groupA\">Methods</h4>\n";

                  foreach(const Proc& p, cl.procs) {
                        out += "<div class=\"method\">\n";

                        QString type(p.type);
                        bool found = false;
                        if (type.endsWith("*")) {
                              type = type.left(type.size()-1);
                              foreach(const Class& cl, classes) {
                                    if (cl.name == type) {
                                          found = true;
                                          break;
                                          }
                                    }
                              }
                        if (found)
                              out += QString("<code><a href=\"%1.html\">%2 </a> ")
                                 .arg(type.toLower()).arg(type);
                        else
                              out += QString("<code>%1 ").arg(type);

                        QRegExp re("([^(]+)\\(([^)]*)\\)");
                        if (re.indexIn(p.name, 0) != -1) {
                              out += QString("<b>%2</b>(%3)</code>\n") .arg(re.cap(1)).arg(re.cap(2));
                              }
                        else {
                              out += QString("<b>%2</b></code>\n").arg(p.name);
                              }
                        out += "</div>\n";
                        if (!p.description.isEmpty()) {
                              out += "<div class=\"mdescr\">\n";
                              foreach(const QString& s, p.description) {
                                    out += s.simplified();
                                    out += "<br/>\n";
                                    }
                              out += "</div>\n";
                              }
                        }
                  }
            if (!cl.props.isEmpty()) {
                  out += "<h4 class=\"groupB\">Properties</h4>\n";
                  out += "<div class=\"tab2\">\n";
                  out += "<table border=\"1\" rules=\"all\" cellpadding=\"0\" cellspacing=\"0\">\n";
                  foreach(const Prop& m, cl.props) {
                        out += "<tr>";
                        out += QString("<td><code>%1</code></td>"
                               "<td><code>%2</code></td>"
                               "<td>%3</td>")
                               .arg(m.name).arg(m.type).arg(m.description);
                        out += "</tr>\n";
                        }
                  out += "</table></div>\n";
                  }
            addFooter(out);

            QString ofile = srcPath + "/share/manual/plugins/" + cl.name.toLower() + ".html";
            QFile of(ofile);
            if (!of.open(QIODevice::WriteOnly)) {
                  printf("open <%s> failed\n", qPrintable(ofile));
                  exit(-4);
                  }
            of.write(out.toUtf8());
            of.close();
            }

      //
      //  write index
      //
      QString out;
      addHeader(out);
      out += "<h2>Objects</h2>\n"
             "<ul>\n";
      qSort(classes);
      foreach(const Class& s, classes) {
            out += QString("<li><a href=\"%1\">%2</a></li>\n")
                    .arg(s.name.toLower() + ".html").arg(s.name);
            }
      addFooter(out);

      QString ofile = srcPath + "/share/manual/plugins/plugins.html";
      QFile of(ofile);
      if (!of.open(QIODevice::WriteOnly)) {
            printf("open <%s> failed\n", qPrintable(ofile));
            exit(-4);
            }
      of.write(out.toUtf8());
      of.close();
      }

//---------------------------------------------------------
//   usage
//---------------------------------------------------------

static void usage(const char* program, const char* hint)
      {
      printf("%s: %s\n", program, hint);
      printf("usage: %s [options] srcPath\n", program);
      printf("options: -v        print version\n"
            );
      }

//---------------------------------------------------------
//   printVersion
//---------------------------------------------------------

static void printVersion(const char* program)
      {
      printf("this is %s, version 0.1\n", program);
      }

//---------------------------------------------------------
//   main
//---------------------------------------------------------

int main(int argc, char* argv[])
      {
      char* prog = argv[0];
      int c;

      while ((c = getopt(argc, argv, "v")) != EOF) {
            switch (c) {
                  case 'v':
                        printVersion(argv[0]);
                        return 0;
                  default:
                        usage(prog, "bad argument");
                        return -1;
                  }
            }
      argc -= optind;
      argv += optind;
      if (argc != 1) {
            usage(prog, "bad arguments");
            return -1;
            }
      srcPath = argv[0];
      QStringList files;
      files << "libmscore/part.h";
      files << "libmscore/staff.h";
      files << "libmscore/element.h";
      files << "libmscore/score.h";
      files << "libmscore/measurebase.h";
      files << "libmscore/measure.h";
      files << "libmscore/segment.h";
      files << "libmscore/chordrest.h";
      files << "libmscore/chord.h";
      files << "libmscore/note.h";
      files << "libmscore/rest.h";
      files << "libmscore/harmony.h";
      files << "libmscore/text.h";
      files << "libmscore/timesig.h";
      files << "libmscore/page.h";
      files << "libmscore/cursor.h";
      files << "libmscore/notedot.h";
      files << "libmscore/slur.h";
      files << "mscore/plugins.h";

      foreach(const QString& s, files) {
            QString infile = srcPath + "/" + s;
            QFile inFile(infile);
            if (!inFile.open(QIODevice::ReadOnly)) {
                  printf("cannot open input file <%s>\n", qPrintable(infile));
                  return -2;
                  }
            printf("ScanFile %s\n", qPrintable(infile));
            QString in = inFile.readAll();
            scanFile(in);
            inFile.close();
            }
      writeOutput();
      return 0;
      }

