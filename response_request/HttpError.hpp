/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpError.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpisak-l <bpisak-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/19 10:56:34 by bpisak-l          #+#    #+#             */
/*   Updated: 2024/11/13 15:49:58 by bpisak-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <exception>
#include <map>

class HttpError : public std::exception
{
private:
	std::string _message;
	int _code;
	std::map<std::string, std::string> _extraHeaderFields;

public:
	HttpError();
	HttpError(const std::string &msg, const int code,
			  const std::map<std::string, std::string> &extraHeaderFields = {});
	HttpError(HttpError const &ex);
	HttpError &operator=(const HttpError &ex);
	std::string const &getMessage() const;
	const std::map<std::string, std::string> &getExtraFields() const;
	int getCode() const;
	virtual ~HttpError() noexcept override;
	const char *what(void) const noexcept override;
	void setField(std::string key, std::string value);
};