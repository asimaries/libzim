/*
 * Copyright (C) 2006,2009 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <zim/file.h>
#include <zim/article.h>
#include "log.h"
#include <zim/fileiterator.h>

log_define("zim.file")

namespace zim
{
  Article File::getArticle(size_type idx) const
  {
    return Article(*this, idx);
  }

  Article File::getArticle(char ns, const std::string& url)
  {
    log_trace("File::getArticle('" << ns << "', \"" << url << ')');
    std::pair<bool, const_iterator> r = findx(ns, url);
    return r.first ? *r.second : Article();
  }

  Article File::getArticleByTitle(size_type idx)
  {
    return Article(*this, impl->getIndexByTitle(idx));
  }

  Article File::getArticleByTitle(char ns, const std::string& title)
  {
    log_trace("File::getArticleByTitle('" << ns << "', \"" << title << ')');
    std::pair<bool, const_iterator> r = findxByTitle(ns, title);
    return r.first ? *r.second : Article();
  }

  bool File::hasNamespace(char ch)
  {
    size_type off = getNamespaceBeginOffset(ch);
    return off < getCountArticles() && getDirent(off).getNamespace() == ch;
  }

  File::const_iterator File::begin()
  { return const_iterator(this, 0); }

  File::const_iterator File::beginByTitle()
  { return const_iterator(this, 0, const_iterator::ArticleIterator); }

  File::const_iterator File::end()
  { return const_iterator(this, getCountArticles()); }

  std::pair<bool, File::const_iterator> File::findx(char ns, const std::string& url)
  {
    log_debug("find article by url " << ns << " \"" << url << "\",  in file \"" << getFilename() << '"');

    size_type l = getNamespaceBeginOffset(ns);
    size_type u = getNamespaceEndOffset(ns);

    if (l == u)
    {
      log_debug("namespace " << ns << " not found");
      return std::pair<bool, const_iterator>(false, end());
    }

    unsigned itcount = 0;
    while (u - l > 1)
    {
      ++itcount;
      size_type p = l + (u - l) / 2;
      Dirent d = getDirent(p);

      int c = ns < d.getNamespace() ? -1
            : ns > d.getNamespace() ? 1
            : url.compare(d.getUrl());

      if (c < 0)
        u = p;
      else if (c > 0)
        l = p;
      else
      {
        log_debug("article found after " << itcount << " iterations in file \"" << getFilename() << "\" at index " << p);
        return std::pair<bool, const_iterator>(true, const_iterator(this, p));
      }
    }

    Dirent d = getDirent(l);
    int c = url.compare(d.getUrl());

    if (c == 0)
    {
      log_debug("article found after " << itcount << " iterations in file \"" << getFilename() << "\" at index " << l);
      return std::pair<bool, const_iterator>(true, const_iterator(this, l));
    }

    log_debug("article not found after " << itcount << " iterations (\"" << d.getUrl() << "\" does not match)");
    return std::pair<bool, const_iterator>(false, const_iterator(this, u));
  }

  std::pair<bool, File::const_iterator> File::findxByTitle(char ns, const std::string& title)
  {
    log_debug("find article by title " << ns << " \"" << title << "\",  in file \"" << getFilename() << '"');

    size_type l = getNamespaceBeginOffset(ns);
    size_type u = getNamespaceEndOffset(ns);

    if (l == u)
    {
      log_debug("namespace " << ns << " not found");
      return std::pair<bool, const_iterator>(false, end());
    }

    unsigned itcount = 0;
    while (u - l > 1)
    {
      ++itcount;
      size_type p = l + (u - l) / 2;
      Dirent d = getDirentByTitle(p);

      int c = ns < d.getNamespace() ? -1
            : ns > d.getNamespace() ? 1
            : title.compare(d.getTitle());

      if (c < 0)
        u = p;
      else if (c > 0)
        l = p;
      else
      {
        log_debug("article found after " << itcount << " iterations in file \"" << getFilename() << "\" at index " << p);
        return std::pair<bool, const_iterator>(true, const_iterator(this, p, const_iterator::ArticleIterator));
      }
    }

    Dirent d = getDirentByTitle(l);
    int c = title.compare(d.getTitle());

    if (c == 0)
    {
      log_debug("article found after " << itcount << " iterations in file \"" << getFilename() << "\" at index " << l);
      return std::pair<bool, const_iterator>(true, const_iterator(this, l, const_iterator::ArticleIterator));
    }

    log_debug("article not found after " << itcount << " iterations (\"" << d.getTitle() << "\" does not match)");
    return std::pair<bool, const_iterator>(false, const_iterator(this, u, const_iterator::ArticleIterator));
  }

  File::const_iterator File::find(char ns, const std::string& url)
  { return findx(ns, url).second; }

  File::const_iterator File::findByTitle(char ns, const std::string& title)
  { return findxByTitle(ns, title).second; }

}