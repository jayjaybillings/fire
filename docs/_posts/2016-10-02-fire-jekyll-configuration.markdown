---
layout: post
title: Fire Jekyll Configuration
permalink: /about/jekyll_config
category: about
---

Fire uses Jekyll for publishing documentation on GitHub Pages and it uses the
[Gravity theme][1]. There are several important things to keep in
mind for this configuration. The Gemfile should contain

```ruby
source 'https://rubygems.org'

# Ascii Doctor and PlantUML
gem 'jekyll', '~> 2.5'
gem 'asciidoctor', '~> 1.5'
gem 'coderay', '1.1.0'
gem 'rake-jekyll', '~> 1.0'
gem 'jekyll-plantuml', '~> 1.1' 

group :jekyll_plugins do
  gem "jekyll-asciidoc"
  gem 'asciidoctor-diagram' 
end
```

I ran into two major problems with GitHub that took hours to fix and then only
after contacting GitHub directly.

- The site.url (or just "url") and site.baseurl variables in _config.yml must be set to 
"http://[username].github.io/[project]" for a project level deployment as in

```
url = "http://jayjaybillings.github.io"
baseurl = "/fire"
```

- The only valid places to put github.io files are in the master branch, master/docs
or in gh-pages. I used gh-pages/doc and it would not work until I merged
into master/docs.

The Gravity theme works as expected when deployed using

```bash
bundle exec jekyll serve
```

but deployment to GitHub fails because the style.scss file has references to
empty scss files in _ssas directory starting at line 47. When these imports
are removed or commented out, GitHub will build the site. 

Acknowledgements
--

Thanks to @AlexPawlowski_ for encouraging me to not give up and sharing some expertise.
Thanks also to Shawna Jean from GitHub for answering my ticket on a Sunday.

[1]: http://hemangsk.github.io/Gravity/
